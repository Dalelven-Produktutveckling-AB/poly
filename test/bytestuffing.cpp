#include <gtest/gtest.h>

#include <poly/com/bytestuffing.hpp>

#include <vector>

TEST(Bytestuffing, Default)
{
    auto stuffed_value = [](uint8_t v) -> uint8_t {
        return ~v;
    };
    std::vector<uint8_t> stuffed_data;
    std::vector<uint8_t> expected_stuffed{1,4,stuffed_value(2),4,stuffed_value(3), 4, stuffed_value(4), 5, 6};
    std::vector<uint8_t> unstuffed{1,2,3,4,5,6};

    auto stuffer = poly::com::default_stuffer([&](uint8_t b) {
        stuffed_data.push_back(b);
    });
    stuffer(unstuffed.begin(), unstuffed.end());
    EXPECT_EQ(stuffed_data, expected_stuffed);

    std::vector<uint8_t> unstuffer_output;
    auto unstuffer = poly::com::default_unstuffer([&](uint8_t b) {
        unstuffer_output.push_back(b);
    });
    unstuffer(stuffed_data.begin(), stuffed_data.end());
    EXPECT_EQ(unstuffer_output, unstuffed);
}

TEST(Bytestuffing, Legacy)
{
    std::vector<uint8_t> stuffed_data;
    std::vector<uint8_t> expected_stuffed;
    std::vector<uint8_t> unstuffed{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    for(auto b: unstuffed)
    {
        if(b <= 0x10)
        {
            expected_stuffed.push_back(0x10);
            expected_stuffed.push_back(b+0x10);
        }
        else
        {
            expected_stuffed.push_back(b);
        }
    }

    auto stuffer = poly::com::legacy_stuffer([&](uint8_t b) {
        stuffed_data.push_back(b);
    });
    stuffer(unstuffed.begin(), unstuffed.end());
    EXPECT_EQ(stuffed_data, expected_stuffed);

    std::vector<uint8_t> unstuffer_output;
    auto unstuffer = poly::com::legacy_unstuffer([&](uint8_t b) {
        unstuffer_output.push_back(b);
    });
    unstuffer(stuffed_data.begin(), stuffed_data.end());
    EXPECT_EQ(unstuffer_output, unstuffed);
}