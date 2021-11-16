#pragma once

#include <poly/utility.hpp>
#include <poly/type_traits.hpp>
#include <poly/function.hpp>
#include <cstdint>

namespace poly::com
{
/**
 * @brief Default bytestuffer
 * @tparam SinkFunction Sink function to write bytes to
 *
 * This byte stuffer will stuff 0x02, 0x03 and 0x04 by prefixing those values
 * with 0x04 and then inverting the actual value.
 *
 * For instance [0x02, 0x03, 0x04, 0x05] will be bytestuffed as [0x04, 0xFD, 0x04, 0xFC, 0x04, 0xFB, 0x05].
 *
 * When stuffing bytes all raw bytes are written to the sink function, so in the above case sink will be called
 * with each of the bytestuffed values in turn.
 */
template<class SinkFunction>
class default_stuffer
{
    SinkFunction sink_;
    static_assert(poly::is_invocable_v<SinkFunction, uint8_t>, "Sink function must take a uint8_t as an argument");
public:
    static constexpr uint8_t STX = 0x02;
    static constexpr uint8_t ETX = 0x03;
    static constexpr uint8_t DLE = 0x04;
    explicit default_stuffer(SinkFunction sink): sink_(poly::move(sink)) {}

    void raw_sink(uint8_t byte) {
        sink_(byte);
    }

    void operator()(uint8_t byte)
    {
        if(byte == STX || byte == ETX || byte == DLE)
        {
            sink_(DLE);
            sink_(~byte);
        }
        else
        {
            sink_(byte);
        }
    }

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        while(begin != end)
        {
            (*this)(*begin);
            ++begin;
        }
    }
};

/**
 * @brief Default byte unstuffer
 * @tparam SinkFunction Sink function to write bytes to
 *
 * This unstuffer will unstuff values prefixed by 0x04 by inverting them.
 *
 * For instance [0x04, 0xFD, 0x04, 0xFC, 0x04, 0xFB, 0x05] will be unstuffed to [0x02, 0x03, 0x04, 0x05].
 *
 * When unstuffing bytes all unstuffed bytes are written to the sink function, so in the above case sink will be called
 * with each of the unstuffed values in turn.
 */
template<class SinkFunction>
class default_unstuffer
{
    SinkFunction sink_;
    bool unstuff_next_ = false;
    static_assert(poly::is_invocable_v<SinkFunction, uint8_t>, "Sink function must take a uint8_t as an argument");
public:
    static constexpr uint8_t DLE = 0x04;

    explicit default_unstuffer(SinkFunction sink): sink_(poly::move(sink)) {}

    void reset() {
        unstuff_next_ = false;
    }

    [[nodiscard]] bool need_more_data() const {
        return unstuff_next_;
    }

    void operator()(uint8_t byte)
    {
        if(byte == DLE)
        {
            unstuff_next_ = true;
        }
        else
        {
            if(unstuff_next_)
            {
                unstuff_next_ = false;
                byte = ~byte;
            }
            sink_(byte);
        }
    }

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        while(begin != end)
        {
            (*this)(*begin);
            ++begin;
        }
    }
};

/**
 * @brief Legacy stuffer stuffing values by prefixing with 0x10 and adding 0x10 to the value.
 * @tparam SinkFunction The sink function to write stuffed values to.
 *
 * This stuffer will prefix all values less than or equal to 0x10 with 0x10 followed by value + 0x10.
 * [0, 1, 2, 0x10, 0x11] will be stuffed to [0x10, 0x10, 0x10, 0x11, 0x10, 0x12, 0x10, 0x20, 0x11].
 */
template<class SinkFunction>
class legacy_stuffer
{
    SinkFunction sink_;
    static_assert(poly::is_invocable_v<SinkFunction, uint8_t>, "Sink function must take a uint8_t as an argument");
public:
    static constexpr uint8_t DLE = 0x10;
    explicit legacy_stuffer(SinkFunction sink): sink_(poly::move(sink)) {}

    void raw_sink(uint8_t byte) {
        sink_(byte);
    }

    void operator()(uint8_t byte)
    {
        if(byte <= DLE)
        {
            sink_(DLE);
            sink_(byte + DLE);
        }
        else
        {
            sink_(byte);
        }
    }

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        while(begin != end)
        {
            (*this)(*begin);
            ++begin;
        }
    }
};

/**
 * @brief Unstuffer for the legacy stuffing scheme
 * @tparam SinkFunction Function to call with unstuffed data.
 *
 * [0x10, 0x10, 0x10, 0x11, 0x10, 0x12, 0x10, 0x20, 0x11] will be unstuffed to [0, 1, 2, 0x10, 0x11].
 */
template<class SinkFunction>
class legacy_unstuffer
{
    SinkFunction sink_;
    bool unstuff_next_ = false;

    static_assert(poly::is_invocable_v<SinkFunction, uint8_t>, "Sink function must take a uint8_t as an argument");
public:
    static constexpr uint8_t DLE = 0x10;

    explicit legacy_unstuffer(SinkFunction sink): sink_(poly::move(sink)) {}

    void reset() {
        unstuff_next_ = false;
    }

    [[nodiscard]] bool need_more_data() const {
        return unstuff_next_;
    }

    void operator()(uint8_t byte)
    {
        if(!unstuff_next_ && byte == DLE)
        {
            unstuff_next_ = true;
        }
        else
        {
            if(unstuff_next_)
            {
                byte -= DLE;
                unstuff_next_ = false;
            }
            sink_(byte);
        }
    }

    template<class IterBegin, class IterEnd>
    void operator()(IterBegin begin, IterEnd end)
    {
        while(begin != end)
        {
            (*this)(*begin);
            ++begin;
        }
    }
};

}