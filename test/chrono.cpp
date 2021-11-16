#include <gtest/gtest.h>

#include <poly/chrono.hpp>

TEST(Chrono, SystemClockConversion)
{
    // Below epoch corresponds to 	Fri Nov 06 11:44:39.876 2020 UTC
    poly::chrono::system_clock::time_point some_time(1604663079876_ms);
    auto days = poly::chrono::floor<poly::chrono::days>(some_time);
    poly::chrono::year_month_day ymd(days);
    poly::chrono::hh_mm_ss time(floor<poly::chrono::milliseconds>(some_time - days));
    EXPECT_EQ((int)ymd.year(), 2020);
    EXPECT_EQ((unsigned)ymd.month(), 11);
    EXPECT_EQ((unsigned)ymd.day(), 6);
    EXPECT_EQ((unsigned)ymd.day(), 6);
    EXPECT_EQ(time.hours().count(), 11);
    EXPECT_EQ(time.minutes().count(), 44);
    EXPECT_EQ(time.seconds().count(), 39);
    EXPECT_EQ(time.subseconds().count(), 876);
}