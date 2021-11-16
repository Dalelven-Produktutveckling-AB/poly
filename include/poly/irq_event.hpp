#pragma once

#include "etl/atomic.h"
#include "etl/optional.h"

#include "detail/irq_event_base.hpp"
#include "irq_event_runtime.hpp"

#include <assert.h>

namespace poly
{
/**
 * @brief A holder for an interrupt event. IRQ events are posted to a runtime for further handling.
 * @tparam Data The data stored by this IRQ event.
 */
template<class Data>
class irq_event final: public detail::irq_event_base
{
    etl::optional<Data> data_;
    etl::atomic<bool> lock_{};
    etl::atomic<bool> is_posted_{};
    void (*cb_)(etl::optional<Data>) = nullptr;
    irq_event_runtime *rt_ = nullptr;

    etl::optional<Data> try_take_data()
    {
        bool expected = false;
        if(lock_.compare_exchange_strong(expected, true))
        {
            etl::optional<Data> retval = etl::move(data_);
            data_ = etl::optional<Data>{};
            lock_.store(false, etl::memory_order_release);
            return retval;
        }

        return etl::nullopt;
    }

public:
    irq_event() = default;
    /**
     * @brief Constructor for the event.
     * @param rt The runtime associated with this event.
     * @param callback The callback associated with this event. The callback will be called from the runtime.
     */
    irq_event(irq_event_runtime& rt, void (*callback)(etl::optional<Data>)): cb_(callback), rt_(&rt)
    {
        lock_.store(false, etl::memory_order_release);
        is_posted_.store(false, etl::memory_order_release);
    }

    /**
     * @brief Late initialization when event is constructed with default constructor.
     * 
     * @param rt The runtime associated with this event.
     * @param callback The callback associated with this event. The callback will be called from the runtime.
     */
    void late_init(irq_event_runtime& rt, void (*callback)(etl::optional<Data>))
    {
        cb_ = callback;
        rt_ = &rt;
        lock_.store(false, etl::memory_order_release);
        is_posted_.store(false, etl::memory_order_release);
    }

    /**
     * @brief Try to set the stored data form IRQ context.
     * @param data The new data
     * @return true if data was successfully set. Otherwise false.
     */
    bool try_set_data(irq_baton, Data data) {
        bool expected = false;
        if(lock_.compare_exchange_strong(expected, true))
        {
            data_ = etl::move(data);
            lock_.store(false, etl::memory_order_release);
            return true;
        }
        return false;
    }

    /**
     * @brief Try to post the event to the associated runtime.
     * @param baton IRQ baton.
     * @return A result indicating if the post was successful or not.
     *
     */
    void post(irq_baton baton) {
        assert(rt_ != nullptr);

        bool expected = false;
        if(is_posted_.compare_exchange_strong(expected, true))
        {
            rt_->post(baton, *this);
        }
    }

    /**
     * @brief Runs the associated callback. This should not be called from user code.
     */
    void run_callback() override {
        is_posted_.store(false, etl::memory_order_release);
        cb_(try_take_data());
    }
};

/**
 * Void specialization of irq_event.
 */
template<>
class irq_event<void> final: public detail::irq_event_base
{
    void (*cb_)() = nullptr;
    irq_event_runtime *rt_ = nullptr;
    etl::atomic<bool> is_posted_{};
public:
    irq_event() = default;
    irq_event(irq_event_runtime& rt, void (*callback)()): cb_(callback), rt_(&rt)
    {
        is_posted_.store(false, etl::memory_order_release);
    }

    /**
     * @brief Late initialization when event is constructed with default constructor.
     *
     * @param rt The runtime associated with this event.
     * @param callback The callback associated with this event. The callback will be called from the runtime.
     */
    void late_init(irq_event_runtime& rt, void (*callback)())
    {
        cb_ = callback;
        rt_ = &rt;
        is_posted_.store(false, etl::memory_order_release);
    }

    /**
     * @brief Try to post the event to the associated runtime.
     * @param baton IRQ baton.
     * @return A result indicating if the post was successful or not.
     *
     */
    void post(irq_baton baton) {
        assert(rt_ != nullptr);

        bool expected = false;
        if(is_posted_.compare_exchange_strong(expected, true))
        {
            rt_->post(baton, *this);
        }
    }

    /**
     * @brief Runs the associated callback. This should not be called from user code.
     */
    void run_callback() override {
        is_posted_.store(false, etl::memory_order_release);
        cb_();
    }
};
}
