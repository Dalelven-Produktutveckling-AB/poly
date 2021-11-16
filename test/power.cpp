#include <gtest/gtest.h>

#include <poly/power.hpp>

TEST(PolyPower, Default)
{
    EXPECT_EQ(poly::power::requested_power_mode(), poly::power::default_mode);
}

TEST(PolyPower, Ticketing)
{
    {
        auto ticket = poly::power::request_minimum_power_mode(poly::power::power_mode::mode4);
        EXPECT_EQ(poly::power::requested_power_mode(), poly::power::power_mode::mode4);
        ticket = poly::power::request_minimum_power_mode(poly::power::power_mode::mode1);
        EXPECT_EQ(poly::power::requested_power_mode(), poly::power::power_mode::mode1);
    }
    EXPECT_EQ(poly::power::requested_power_mode(), poly::power::default_mode);
    {
        auto ticket = poly::power::request_minimum_power_mode(poly::power::power_mode::mode4);
        EXPECT_EQ(poly::power::requested_power_mode(), poly::power::power_mode::mode4);
        {
            auto ticket2 = poly::power::request_minimum_power_mode(poly::power::power_mode::mode1);
            auto ticket3 = poly::power::request_minimum_power_mode(poly::power::power_mode::mode3);
            EXPECT_EQ(poly::power::requested_power_mode(), poly::power::power_mode::mode1);
        }
        EXPECT_EQ(poly::power::requested_power_mode(), poly::power::power_mode::mode4);
    }
}