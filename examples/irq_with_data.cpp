#include <iostream>
#include <thread>
#include <random>

#include "poly/irq_event_runtime.hpp"
#include "poly/irq_event.hpp"

void write_integer(etl::optional<uint32_t> integer) {
    if(integer) {
        std::cout << "Received " << *integer << std::endl;
    }
}

int main() {
    poly::irq_event_runtime irq_rt;
    poly::irq_event<uint32_t> random_evt(irq_rt, write_integer);

    std::thread random_gen_thread([&random_evt]() {
        std::minstd_rand rand(std::random_device{}());
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto value = rand();
            std::cout << "Sending  " << value << std::endl;
            random_evt.try_set_data(poly::irq_baton{}, value);
            random_evt.post(poly::irq_baton{});
        }
    });

    while(true) {
        irq_rt.run_available();
    }
}