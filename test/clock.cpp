#include "gtest/gtest.h"

#if 0
#include "poly/clock.hpp"

TEST(ClockConfig, Duration)
{
    using clock_def = poly::clock_type<500, 1000, 1, 10, 100, 1000>;
    {
        auto one_sec_def = clock_def::setting_from_duration(1_sec);
        EXPECT_EQ(one_sec_def.divisor, 1000);
        EXPECT_EQ(one_sec_def.target_value, 1);
    }
    {
        auto one_sec_def = clock_def::setting_from_duration(600_ms);
        EXPECT_EQ(one_sec_def.divisor, 10);
        EXPECT_EQ(one_sec_def.target_value, 60);
    }
}
#endif