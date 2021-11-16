#include "poly/irq_event_runtime.hpp"
#include "poly/timer.hpp"

#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <chrono>

struct TimerData {
    std::atomic<bool> should_post;
    std::chrono::steady_clock::time_point start_time;

    TimerData(): should_post(true), start_time(std::chrono::steady_clock::now()) {}
};

struct ThreadTimer: std::enable_shared_from_this<ThreadTimer> {
    std::thread th_;

    void start(void (*irq_cb)(), std::chrono::milliseconds timeout, std::shared_ptr<TimerData> data)
    {
        th_ = std::thread([irq_cb, timeout, me = this->shared_from_this(), data]() {
            std::this_thread::sleep_for(timeout);
            if(data->should_post) {
                irq_cb();
            }
        });
        th_.detach();
    }
};

static std::shared_ptr<TimerData> timer_data_;
static void start(void (*irq_cb)(), std::size_t timeout_ms)
{
    if(timer_data_) {
        timer_data_->should_post.store(false, std::memory_order_release);
    }
    auto shared = std::make_shared<TimerData>();
    auto timer = std::make_shared<ThreadTimer>();
    timer_data_ = shared;
    timer->start(irq_cb, std::chrono::milliseconds(timeout_ms), shared);
}

static std::size_t stop() {
    if(timer_data_) {
        timer_data_->should_post.store(false, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - timer_data_->start_time).count();
    }
    return 0;
}

void timer1_cb(poly::deadline_timer& timer) {
    std::cout << "Timer 1 timeout" << std::endl;
    std::cout << "This thread = " << std::this_thread::get_id() << std::endl;
    timer.async_wait(1_sec);
}

void timer2_cb(poly::deadline_timer& timer) {
    std::cout << "Timer 2 timeout" << std::endl;
    std::cout << "This thread = " << std::this_thread::get_id() << std::endl;
    timer.async_wait(2_sec);
}

int main() {
    poly::timer_clock clk;
    clk.start = start;
    clk.stop = stop;
    poly::irq_event_runtime irq_rt;
    poly::timer_task::init(irq_rt, clk);

    poly::deadline_timer timer1;
    poly::deadline_timer timer2;

    timer1.async_wait(timer1_cb, 1_sec);
    timer2.async_wait(timer2_cb, 1_sec);

    std::cout << "This thread = " << std::this_thread::get_id() << std::endl;
    while(true) {
        irq_rt.run_available();
    }
}
