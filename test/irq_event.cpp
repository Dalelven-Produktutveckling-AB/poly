#include <gtest/gtest.h>

#include "poly/irq_event_runtime.hpp"
#include "poly/irq_event.hpp"

TEST(IrqEvent, Ordering)
{
    static int order = 0;
    poly::irq_event_runtime rt;

    order = 0;

    poly::irq_event<void> event1(rt, []() {
        EXPECT_EQ(order, 0);
        order++;
    });
    poly::irq_event<void> event2(rt, []() {
        EXPECT_EQ(order, 1);
        order++;
    });
    poly::irq_event<void> event3(rt, []() {
        EXPECT_EQ(order, 2);
        order++;
    });

    event1.post(poly::irq_baton{});
    event2.post(poly::irq_baton{});
    event3.post(poly::irq_baton{});

    rt.run_available();
    EXPECT_EQ(order, 3);
}