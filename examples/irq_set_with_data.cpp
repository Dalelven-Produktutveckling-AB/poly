#include <iostream>
#include <thread>

#include "poly/irq_event_runtime.hpp"
#include "poly/irq_event.hpp"
#include "poly/irq_event_set.hpp"

struct EventData {
    int number_;
    EventData(int number): number_(number) {}
};

static void handle_event_data(etl::optional<EventData> data) {
    if (data) {
        std::cout << "Received " << data->number_ << std::endl;
    }
}

int main() {
    poly::irq_event_runtime irq_rt;
    poly::irq_event_set<EventData, 5> evt_set(irq_rt, handle_event_data);

    std::thread event_thread([&evt_set]() {
        int number = 1;

        auto send_event([&evt_set](auto number) {
            std::cout << "Sending " << number << std::endl;
            auto result = evt_set.post(poly::irq_baton{}, EventData(number));

            if (result.is_error())
            {
                std::cout << "Failed to post event";
                auto err = result.maybe_error();
                if (err)
                {
                    std::cout << ": " << std::string(err->string());
                }
                std::cout << std::endl;
            }
        });

        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            send_event(number++);
            send_event(number++);
            send_event(number++);
        }
    });

    while(true) {
        irq_rt.run_available();
    }
}
