#include <gtest/gtest.h>

#include <string.h>
#include <poly/md5.hpp>
#include <poly/iterator.hpp>
#include <poly/algorithm.hpp>
#include <etl/array.h>

static const etl::array<uint8_t, 97> md5_test_data
{0xda,0xda,0xda,0xda,0xda,0xd2,0x34,0x93,0x28,0x28,0x90,0x24,0x80,0x28,0x02,0x34,
0x80,0x89,0x02,0x34,0x09,0x23,0x40,0x82,0x34,0x89,0x04,0x23,0x89,0xfd,0xdf,0xdf,
0x59,0xae,0xfe,0x34,0x23,0x42,0x34,0x23,0x4e,0xfd,0xfd,0xf3,0x34,0x23,0x43,0x2a,
0xdf,0xdf,0xdf,0xdf,0xdf,0xde,0xef,0xb4,0x66,0x76,0x53,0x44,0x54,0x67,0xf4,0x5e,
0x56,0x53,0x4e,0x64,0xe5,0x45,0xe6,0x48,0x56,0x74,0x66,0x54,0x65,0xf5,0x36,0x45,
0x64,0xe6,0x78,0x75,0xe7,0x45,0x67,0x83,0x57,0x58,0xae,0x46,0x57,0x87,0xfe,0xe5,
0x50};

static const poly::hash::md5::digest expected_md5_sum
{0x74, 0xa4, 0xbd, 0x52, 0x08, 0x05, 0x4f, 0xd3, 0x4e, 0xc4, 0xed, 0x29, 0x30, 0x49, 0xcc, 0xf1};

static void block_update_digest(poly::hash::md5& md5)
{
    auto size_left = md5_test_data.size();
    int i = 0;
    while (size_left)
    {
        auto buf_size = (size_left > 10) ? 10 : size_left;
        md5.update(etl::span<const uint8_t>(&md5_test_data[i*10], buf_size));
        size_left -= buf_size;
        i++;
    }
}

TEST(Md5, Checksum)
{
    using namespace poly::hash;

    {
        md5 hasher;
        hasher.reset();

        block_update_digest(hasher);

        auto md5_sum = hasher.finalize();
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), expected_md5_sum.begin()));    

        // Running finalize again should be allowed and return same sum
        auto md5_sum_copy = hasher.finalize();
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), md5_sum_copy.begin()));

        // Update should fail after finalize
        EXPECT_FALSE(hasher.update(etl::span<const uint8_t>(&md5_test_data[0], 10)));

        // Reset and start over
        hasher.reset();
        block_update_digest(hasher);
        md5_sum = hasher.finalize();   
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), expected_md5_sum.begin())); 

        // Back inserter
        hasher.reset();
        poly::copy(md5_test_data.cbegin(), md5_test_data.cend(), poly::back_inserter(hasher));
        md5_sum = hasher.finalize();
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), expected_md5_sum.begin()));
    }

    {
        // operator()
        md5 hasher;
        for (const auto& d : md5_test_data)
        {
            hasher(d);
        }
        auto md5_sum = hasher.finalize();
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), expected_md5_sum.begin()));

        hasher.reset();
        auto size_left = md5_test_data.size();
        int i = 0;
        while (size_left)
        {
            auto buf_size = (size_left > 10) ? 10 : size_left;
            hasher(etl::span<const uint8_t>(&md5_test_data[i*10], buf_size));
            size_left -= buf_size;
            i++;
        }
        md5_sum = hasher.finalize();        
        EXPECT_TRUE(etl::equal(md5_sum.begin(), md5_sum.end(), expected_md5_sum.begin()));
    }
}