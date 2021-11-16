#include "gtest/gtest.h"

#include "poly/bitutil.hpp"

TEST(BitUtil, LittleEndianDecodeUnchecked)
{
    {
        uint8_t buffer[]{1, 2, 3, 4};
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint16_t>({buffer, 4}), 0x0201);
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::int16_t>({buffer, 4}), 0x0201);

        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint32_t>({buffer, 4}), 0x04030201);
    }

    {
        uint8_t buffer[]{0xef,0xcd,0xab,0x80};
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint32_t>({buffer, 4}), 0x80abcdef);
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::int32_t>({buffer, 4}), -2'136'224'273);
    }
}

TEST(BitUtil, LittleEndianDecodeUncheckedIter)
{
    {
        uint8_t buffer[]{1, 2, 3, 4};
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint16_t>(buffer, buffer+4), 0x0201);
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::int16_t>(buffer, buffer+4), 0x0201);

        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint32_t>(buffer, buffer+4), 0x04030201);
    }

    {
        uint8_t buffer[]{0xef,0xcd,0xab,0x80};
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::uint32_t>(buffer, buffer+4), 0x80abcdef);
        EXPECT_EQ(poly::bitutil::little_endian_decode_unchecked<std::int32_t>(buffer, buffer+4), -2'136'224'273);
    }
}

TEST(BitUtil, LittleEndianDecode)
{
    {
        uint8_t buffer[]{1, 2, 3, 4};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint16_t>({buffer, 4}).unwrap(), 0x0201);
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::int16_t>({buffer, 4}).unwrap(), 0x0201);

        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint32_t>({buffer, 4}).unwrap(), 0x04030201);
    }

    {
        uint8_t buffer[]{0xef,0xcd,0xab,0x80};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint32_t>({buffer, 4}).unwrap(), 0x80abcdef);
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::int32_t>({buffer, 4}).unwrap(), -2'136'224'273);
    }
    {
        uint8_t buffer[]{1,2,3};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint16_t>({buffer, 3}).unwrap(), 0x0201);
        EXPECT_TRUE(poly::bitutil::little_endian_decode<std::uint32_t>({buffer, 3}).is_error());
    }
}

TEST(BitUtil, LittleEndianDecodeIter)
{
    {
        uint8_t buffer[]{1, 2, 3, 4};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint16_t>(buffer, buffer+4).unwrap(), 0x0201);
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::int16_t>(buffer, buffer+4).unwrap(), 0x0201);

        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint32_t>(buffer, buffer+4).unwrap(), 0x04030201);
    }

    {
        uint8_t buffer[]{0xef,0xcd,0xab,0x80};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint32_t>(buffer, buffer+4).unwrap(), 0x80abcdef);
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::int32_t>(buffer, buffer+4).unwrap(), -2'136'224'273);
    }
    {
        uint8_t buffer[]{1,2,3};
        EXPECT_EQ(poly::bitutil::little_endian_decode<std::uint16_t>(buffer, buffer+3).unwrap(), 0x0201);
        EXPECT_TRUE(poly::bitutil::little_endian_decode<std::uint32_t>(buffer, buffer+3).is_error());
    }
}

TEST(BitUtil, LittleEndianEncodeUnchecked)
{
    etl::array<uint8_t, 4> buffer{0, 0, 0, 0};
    auto expect = [&](etl::array<uint8_t, 4> expected) {
        EXPECT_EQ(expected, buffer);
    };
    uint16_t uvalue = 0x0201;
    poly::bitutil::little_endian_encode_unchecked(uvalue, {buffer.data(), 4});
    expect({0x01, 0x02, 0, 0});

    int16_t ivalue = 0x0403;
    poly::bitutil::little_endian_encode_unchecked(ivalue, {buffer.data(), 4});
    expect({0x03, 0x04, 0, 0});

    uint32_t u32 = 0x80abcdef;
    poly::bitutil::little_endian_encode_unchecked(u32, {buffer.data(), 4});
    expect({0xef,0xcd,0xab,0x80});
    buffer.fill(0);

    int32_t  i32 = -2'136'224'273;
    poly::bitutil::little_endian_encode_unchecked(i32, {buffer.data(), 4});
    expect({0xef,0xcd,0xab,0x80});
}

TEST(BitUtil, LittleEndianEncodeUncheckedIter)
{
    etl::array<uint8_t, 4> buffer{0, 0, 0, 0};
    auto expect = [&](etl::array<uint8_t, 4> expected) {
        EXPECT_EQ(expected, buffer);
    };
    uint16_t uvalue = 0x0201;
    poly::bitutil::little_endian_encode_unchecked(uvalue, buffer.begin());
    expect({0x01, 0x02, 0, 0});

    int16_t ivalue = 0x0403;
    poly::bitutil::little_endian_encode_unchecked(ivalue, buffer.begin());
    expect({0x03, 0x04, 0, 0});

    uint32_t u32 = 0x80abcdef;
    poly::bitutil::little_endian_encode_unchecked(u32, buffer.begin());
    expect({0xef,0xcd,0xab,0x80});
    buffer.fill(0);

    int32_t  i32 = -2'136'224'273;
    poly::bitutil::little_endian_encode_unchecked(i32, buffer.begin());
    expect({0xef,0xcd,0xab,0x80});
}

TEST(BitUtil, LittleEndianEncode)
{
    {
        etl::array<uint8_t, 4> buffer{0, 0, 0, 0};
        auto expect = [&](etl::array<uint8_t, 4> expected) {
            EXPECT_EQ(expected, buffer);
        };
        uint16_t uvalue = 0x0201;
        auto res = poly::bitutil::little_endian_encode(uvalue, {buffer.data(), 2});
        EXPECT_TRUE(res.is_ok());
        expect({0x01, 0x02, 0, 0});

        int16_t ivalue = 0x0403;
        res = poly::bitutil::little_endian_encode(ivalue, {buffer.data(), 2});
        EXPECT_TRUE(res.is_ok());
        expect({0x03, 0x04, 0, 0});

        uint32_t u32 = 0x80abcdef;
        res = poly::bitutil::little_endian_encode(u32, {buffer.data(), 4});
        EXPECT_TRUE(res.is_ok());
        expect({0xef,0xcd,0xab,0x80});
        buffer.fill(0);

        int32_t i32 = -2'136'224'273;
        res = poly::bitutil::little_endian_encode(i32, {buffer.data(), 4});
        EXPECT_TRUE(res.is_ok());
        expect({0xef,0xcd,0xab,0x80});
    }
    {
        etl::array<uint8_t, 4> buffer{0, 0, 0, 0};
        auto no_decode = [&]() {
            auto expected = etl::array<uint8_t, 4>{0,0,0,0};
            EXPECT_EQ(expected, buffer);
        };
        uint16_t uvalue = 0x0201;
        auto res = poly::bitutil::little_endian_encode(uvalue, {buffer.data(), 1});
        EXPECT_TRUE(res.is_error());
        no_decode();

        int16_t ivalue = 0x0403;
        res = poly::bitutil::little_endian_encode(ivalue, {buffer.data(), 1});
        EXPECT_TRUE(res.is_error());
        no_decode();

        uint32_t u32 = 0x80abcdef;
        res = poly::bitutil::little_endian_encode(u32, {buffer.data(), 3});
        EXPECT_TRUE(res.is_error());
        no_decode();

        int32_t i32 = -2'136'224'273;
        res = poly::bitutil::little_endian_encode(i32, {buffer.data(), 3});
        EXPECT_TRUE(res.is_error());
        no_decode();
    }
}

TEST(BitUtil, LittleEndianEncodeIter)
{
    {
        etl::array<uint8_t, 4> buffer{0, 0, 0, 0};
        auto expect = [&](etl::array<uint8_t, 4> expected) {
            EXPECT_EQ(expected, buffer);
        };
        uint16_t uvalue = 0x0201;
        auto res = poly::bitutil::little_endian_encode(uvalue, buffer.begin(), buffer.begin() + 2);
        EXPECT_TRUE(res.is_ok());
        expect({0x01, 0x02, 0, 0});

        int16_t ivalue = 0x0403;
        res = poly::bitutil::little_endian_encode(ivalue, buffer.begin(), buffer.begin() + 2);
        EXPECT_TRUE(res.is_ok());
        expect({0x03, 0x04, 0, 0});

        res = poly::bitutil::little_endian_encode(ivalue, buffer.begin(), buffer.end());
        ASSERT_TRUE(res.is_ok());
        EXPECT_EQ(res.unwrap(), buffer.begin() + 2);
        expect({0x03, 0x04, 0, 0});

        uint32_t u32 = 0x80abcdef;
        res = poly::bitutil::little_endian_encode(u32, buffer.begin(), buffer.end());
        EXPECT_TRUE(res.is_ok());
        expect({0xef,0xcd,0xab,0x80});
        buffer.fill(0);

        int32_t i32 = -2'136'224'273;
        res = poly::bitutil::little_endian_encode(i32, buffer.begin(), buffer.end());
        ASSERT_TRUE(res.is_ok());
        EXPECT_EQ(res.unwrap(), buffer.end());
        expect({0xef,0xcd,0xab,0x80});
    }
    {
        etl::array<uint8_t, 4> buffer{0, 0, 0, 0};

        uint16_t uvalue = 0x0201;
        auto res = poly::bitutil::little_endian_encode(uvalue, buffer.begin(), buffer.begin() + 1);
        EXPECT_TRUE(res.is_error());

        int16_t ivalue = 0x0403;
        res = poly::bitutil::little_endian_encode(ivalue, buffer.begin(), buffer.begin() + 1);
        EXPECT_TRUE(res.is_error());

        uint32_t u32 = 0x80abcdef;
        res = poly::bitutil::little_endian_encode(u32, buffer.begin(), buffer.begin() + 3);
        EXPECT_TRUE(res.is_error());

        int32_t i32 = -2'136'224'273;
        res = poly::bitutil::little_endian_encode(i32, buffer.begin(), buffer.begin() + 3);
        EXPECT_TRUE(res.is_error());
    }
}