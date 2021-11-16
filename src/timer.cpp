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

#include "poly/irq_event.hpp"
#include "poly/timer.hpp"
#include "poly/manual_lifetime.hpp"

// Block async_wait from posting the timer_irq_event.
static poly::manual_lifetime<poly::irq_event<void>> timer_irq_event_;
static poly::manual_lifetime<poly::soft_event_service<poly::deadline_timer>> timer_service_;
static poly::irq_event_runtime* rt_ = nullptr;
static poly::timer_clock clk_;

static void clock_irq() {
    if(!rt_) {
        // Not initialized!
        return;
    }
    timer_irq_event_->post(poly::irq_baton{});
}

static void irq_event_callback() {
    if(!rt_) {
        return;
    }

    poly::deadline_timer::duration timeout(static_cast<int64_t>(clk_.stop()));

    etl::optional<poly::deadline_timer::duration> next_timeout;
    auto maybe_update_next_timeout = [&next_timeout](poly::deadline_timer::duration dur) {
        if(dur == 0_ms)
        {
            return;
        }

        if(next_timeout) {
            if(dur < *next_timeout) {
                *next_timeout = dur;
            }
        }
        else {
            next_timeout = dur;
        }
    };

    timer_service_->notify_active_listeners([&](poly::deadline_timer& timer) {
        if(timeout > timer.get_timeout()) {
            timer.set_timeout(0_ms);
        }
        else {
            timer.set_timeout(timer.get_timeout() - timeout);
        }

        maybe_update_next_timeout(timer.get_timeout());

        return timer.get_timeout() == 0_ms;
    });

    timer_service_->add_pending_listeners([&](const poly::deadline_timer &timer) {
        maybe_update_next_timeout(timer.get_timeout());
        return true;
    });

    if(next_timeout) {
        clk_.start(clock_irq, next_timeout->count());
    }
}

namespace poly::timer_task
{
void init(irq_event_runtime& rt, timer_clock clk)
{
    rt_ = &rt;
    clk_ = clk;
    timer_irq_event_.emplace(rt, irq_event_callback);
    timer_service_.emplace(*timer_irq_event_);
}
}

namespace poly
{
void deadline_timer::async_wait(duration timeout)
{
    if(!rt_) {
        return;
    }
    if(timeout.count() == 0)
    {
        timeout = duration(1);
    }

    until_timeout_ = timeout;
    timer_service_->add_listener(*this);
}
}
