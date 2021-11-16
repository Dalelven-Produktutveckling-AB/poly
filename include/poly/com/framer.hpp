#pragma once

#include <poly/crc.hpp>
#include <poly/utility.hpp>

#include <etl/deque.h>
#include <etl/optional.h>

namespace poly::com
{
template<template<class> class Stuffer, class SinkFunction>
class stx_etx_framer
{
    static constexpr uint8_t STX = 0x02;
    static constexpr uint8_t ETX = 0x03;

    Stuffer<SinkFunction> stuffer_;
    poly::crc::crc16_ccitt crc_;
    enum class state_type: uint8_t
    {
        started,
        finished,
        closed,
    };
    state_type state_ = state_type::started;
public:
    explicit stx_etx_framer(SinkFunction sink): stuffer_(poly::move(sink)) {
        stuffer_.raw_sink(STX);
    }

    stx_etx_framer(const stx_etx_framer&) = delete;
    stx_etx_framer& operator=(const stx_etx_framer&) = delete;

    stx_etx_framer(stx_etx_framer&& rhs)  noexcept {
        stuffer_ = poly::move(rhs.stuffer_);
        crc_ = rhs.crc_;
        state_ = rhs.state_;
        rhs.close();
    }

    stx_etx_framer& operator=(stx_etx_framer&& rhs)  noexcept {
        if(this == &rhs) {
            return *this;
        }

        stuffer_ = poly::move(rhs.stuffer_);
        crc_ = rhs.crc_;
        state_ = rhs.state_;
        rhs.close();
        return *this;
    }

    ~stx_etx_framer() {
        finish();
    }

    void close() {
        state_ = state_type::closed;
    }

    void finish() {
        if(state_ == state_type::started) {
            state_ = state_type::finished;
            auto crc = crc_.crc();
            stuffer_(crc & 0x00FFu);
            stuffer_(crc >> 8u);
            stuffer_.raw_sink(ETX);
        }
    }

    [[nodiscard]] bool is_finished() const {
        return state_ == state_type::finished;
    }

    [[nodiscard]] bool is_closed() const {
        return state_ == state_type::closed;
    }

    void operator()(uint8_t byte) {
        if(!is_finished() && !is_closed()) {
            crc_ = crc_(byte);
            stuffer_(byte);
        }
    }

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        if(is_finished() || is_closed()) {
            return;
        }

        while(begin != end)
        {
            (*this)(*begin);
            ++begin;
        }
    }
};

struct framed_byte
{
    enum class error_type
    {
        bad_crc,
        bad_framing,
        bad_bytestuffing,
    };
    etl::optional<error_type> error;
    etl::optional<uint8_t> byte;
    etl::optional<size_t> consumed_length;

    [[nodiscard]] bool frame_is_done() const {
        return !byte.has_value() && !frame_is_error();
    }

    [[nodiscard]] bool frame_is_error() const {
        return error.has_value();
    }
};

template<template<class> class UnStuffer, class SinkFunction>
class stx_etx_deframer
{
    static constexpr uint8_t STX = 0x02;
    static constexpr uint8_t ETX = 0x03;

    static_assert(poly::is_invocable_v<SinkFunction, framed_byte>, "Sink function must take a framed_byte as its argument");
    struct unstuffer_sink
    {
        stx_etx_deframer<UnStuffer, SinkFunction>* deframer_;
        void operator()(uint8_t byte)
        {
            if(deframer_->history_.full())
            {
                auto front = deframer_->history_.front();
                deframer_->history_.pop_front();
                deframer_->history_.push_back(byte);

                framed_byte to_sink;
                to_sink.byte = front;

                deframer_->sink_(to_sink);
                deframer_->crc_ = deframer_->crc_(front);
            }
            else {
                deframer_->history_.push_back(byte);
            }
        }
    };

    SinkFunction sink_;
    etl::deque<uint8_t, 2> history_;
    UnStuffer<unstuffer_sink> unstuffer_;
    poly::crc::crc16_ccitt crc_;
    bool stx_found_ = false;

    void reset()
    {
        crc_ = poly::crc::crc16_ccitt();
        history_.clear();
        unstuffer_ = UnStuffer<unstuffer_sink>(unstuffer_sink{this});
    }

public:
    explicit stx_etx_deframer(SinkFunction sink): sink_(poly::move(sink)), unstuffer_(unstuffer_sink{this}) {}

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        size_t len = 0;
        while(begin != end)
        {
            len++;
            if(!stx_found_)
            {
                if(*begin == STX)
                {
                    stx_found_ = true;
                    reset();
                }
            }
            else
            {
                if(*begin == ETX)
                {
                    stx_found_ = false;
                    if(unstuffer_.need_more_data())
                    {
                        framed_byte result;
                        result.error = framed_byte::error_type::bad_bytestuffing;
                        sink_(result);
                    }
                    else if(!history_.full())
                    {
                        framed_byte result;
                        result.error = framed_byte::error_type::bad_framing;
                        sink_(result);
                    }
                    else
                    {
                        auto crc = crc_.crc();
                        uint8_t lsb_crc = crc & 0x00FFu;
                        uint8_t msb_crc = crc >> 8u;
                        if(lsb_crc != history_[0] || msb_crc != history_[1])
                        {
                            framed_byte result;
                            result.error = framed_byte::error_type::bad_crc;
                            sink_(result);
                        }
                        else
                        {
                            framed_byte result;
                            result.consumed_length = len;
                            len = 0;
                            sink_(result);
                        }
                    }
                }
                else
                {
                    if(*begin == STX)
                    {
                        framed_byte result;
                        result.error = framed_byte::error_type::bad_framing;
                        sink_(result);
                        reset();
                    }
                    else
                    {
                        unstuffer_(*begin);
                    }
                }
            }
            ++begin;
        }
    }
};

template<template<class> class Stuffer, class SinkFunction>
stx_etx_framer<Stuffer, SinkFunction> make_stx_etx_framer(SinkFunction sink) {
    return stx_etx_framer<Stuffer, SinkFunction>{poly::move(sink)};
}

template<template<class> class Stuffer, class SinkFunction>
stx_etx_deframer<Stuffer, SinkFunction> make_stx_etx_deframer(SinkFunction sink) {
    return stx_etx_deframer<Stuffer, SinkFunction>{poly::move(sink)};
}

}