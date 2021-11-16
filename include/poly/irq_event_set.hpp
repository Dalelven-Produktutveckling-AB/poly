/**
* Copyright © 2021 Dalelven Produktutveckling AB
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
* IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "etl/array.h"
#include "etl/queue_spsc_atomic.h"

#include "irq_event.hpp"
#include "irq_event_runtime.hpp"

#include <cstddef>

namespace poly
{
/**
 * @brief A holder for a set of IRQ events.
 * @tparam T The data that can be stored in each IRQ event.
 * @tparam Capacity Number of IRQ events in the set.
 */
template<class T, std::size_t Capacity>
class irq_event_set
{
    struct irq_event_set_data
    {
        irq_event_set<T, Capacity>* event_set;
        irq_event<irq_event_set_data>* event;
        etl::optional<T> data;
    };

    void (*event_callback_)(etl::optional<T>);

    etl::array<irq_event<irq_event_set_data>, Capacity> events_;
    etl::queue_spsc_atomic<irq_event<irq_event_set_data>*, Capacity> available_events_;

    static void internal_rt_callback(etl::optional<irq_event_set_data> evt_set_data)
    {
        if (evt_set_data)
        {
            evt_set_data->event_set->run_event_callback(etl::move(*evt_set_data));
        }
    }

    void run_event_callback(irq_event_set_data evt_set_data)
    {
        available_events_.push(evt_set_data.event);
        event_callback_(etl::move(evt_set_data.data));
    }

public:
    irq_event_set() = default;
    irq_event_set(const irq_event_set&) = delete;
    irq_event_set(irq_event_set&&) = delete;
    irq_event_set& operator=(const irq_event_set&) = delete;
    irq_event_set& operator=(irq_event_set&&) = delete;

    /**
     * @brief Constructor for the event set.
     * @param rt The runtime associated with the events.
     * @param callback The callback associated with the events. The callback will be called from the runtime.
     */
    irq_event_set(irq_event_runtime& rt, void (*callback)(etl::optional<T>)) : event_callback_(callback)
    {
        for (auto &e : events_)
        {
            e.late_init(rt, internal_rt_callback);
            available_events_.push(&e);
        }
    }

    /**
     * @brief Late initialization when event set is constructed with default constructor.
     *
     * @param rt The runtime associated with this event.
     * @param callback The callback associated with this event set. The callback will be called from the runtime.
     */
    void late_init(irq_event_runtime& rt, void (*callback)(etl::optional<T>))
    {
        event_callback_ = callback;

        for (auto &e : events_)
        {
            e.late_init(rt, internal_rt_callback);
            available_events_.push(&e);
        }
    }

    /**
     * @brief Try to post an event to the event set.
     * @param baton IRQ baton.
     * @param data Event data.
     * @return A result indicating if the post was successful or not.
     *
     */
    poly::result<void, string_literal> post(irq_baton baton, T data)
    {
        irq_event<irq_event_set_data>* evt;
        bool found = available_events_.pop(evt);

        if (found)
        {
            irq_event_set_data set_data;
            set_data.event_set = this;
            set_data.event = evt;
            set_data.data = etl::move(data);
            if (!evt->try_set_data(baton, etl::move(set_data)))
            {
                return poly::error("Failed to set event data"_str);
            }

            evt->post(baton);
            return poly::ok();
        }
        else
        {
            return poly::error("Event set full"_str);
        }
    }
};
}
