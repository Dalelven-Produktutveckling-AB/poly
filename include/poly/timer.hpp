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

#include "irq_event_runtime.hpp"
#include "soft_event.hpp"
#include "chrono.hpp"
#include "function.hpp"

namespace poly
{
/**
 * @brief A simple deadline timer that can be used to detect and handle timeouts.
 */
class deadline_timer final: public soft_event_base
{
public:
    /**
     * The duration type used by this deadline timer.
     */
    using duration = chrono::milliseconds;
private:
    duration until_timeout_ = 0_ms;
    poly::function<void(deadline_timer&)> callback_{};
public:
    /**
     * @brief Changes the handler of this callback without starting to wait for a timeout.
     * @param callback The new handler to use.
     */
    void set_handler(const poly::function<void(deadline_timer&)>& callback) {
        callback_ = callback;
    }

    /**
     * @brief Start waiting for a timeout to occur. If a handler is set this handler will be called after timeout.
     * @param timeout The timeout to wait for.
     */
    void async_wait(duration timeout);

    /**
     * @brief Start waiting for a timeout, using the specified callback.
     * @param callback The new callback to use.
     * @param timeout The timeout of this deadline timer.
     */
    void async_wait(const poly::function<void(deadline_timer&)>& callback, duration timeout) {
        set_handler(callback);
        async_wait(timeout);
    }

    /**
     * @brief Cancel the timer. This will not call the callback.
     */
    void cancel() {
        if(is_linked()) {
            safe_unlink();
        }
    }

    /**
     * @brief Set a new timeout. This should normally not be called by user code.
     * @param timeout The new timeout to use.
     */
    void set_timeout(duration timeout) {
        until_timeout_ = timeout;
    }

    /**
     * @brief Get the stored time until timeout. This should normally not be called by user code.
     * @return The currently stored time until timeout.
     */
    [[nodiscard]] duration get_timeout() const {
        return until_timeout_;
    }

    /**
     * @brief Notify the callback that timeout has occurred. This should normally not be called by user code.
     */
    void notify() {
        callback_(*this);
    }
};

/**
 * @brief A minimal timer interface used by the timer task.
 *
 * This is implemented as a C struct so that it can be used to interface
 * with existing C code and drivers.
 */
struct timer_clock {
    /**
     * @brief Function pointer to a start function.
     * @param irq_callback The interrupt callback. External code must call this function after timeout_ms milliseconds.
     * @param timeout_ms The timeout in milliseconds.
     *
     * `irq_callback` must be called by external code after timeout_ms milliseconds has elapsed.
     * unless the `stop` function has completed.
     */
    void (*start)(void (*irq_callback)(), size_t timeout_ms) = nullptr;
    /**
     * @brief Stops the timer.
     * @return The number of milliseconds elapsed since `start` was called, or 0 if `start` wasn't called before `stop`.
     *
     * When this function has finished, `irq_callback` must not be called.
     */
    size_t (*stop)() = nullptr;
};

namespace timer_task
{
/**
 * @brief Initialize the timer task. This is used to drive all deadline timers.
 * @param rt The IRQ runtime to use.
 * @param clk Clock interface to use.
 *
 * This must be called before any `deadline_timer` starts waiting.
 */
void init(irq_event_runtime& rt, timer_clock clk);
}
}