#include <gtest/gtest.h>

#include <poly/com/framer.hpp>
#include <poly/com/bytestuffing.hpp>

#include <vector>


TEST(StxEtxFramer, Framing)
{
    std::vector<uint8_t> bytes;
    {
        auto framer = poly::com::make_stx_etx_framer<poly::com::default_stuffer>([&](uint8_t byte) {
            bytes.push_back(byte);
        });

        framer(0x04);
        framer.finish();
        ASSERT_EQ(bytes.size(), 1 + 1 + 1 + 2 + 1); // STX + DLE + byte + CRC + ETX
        EXPECT_EQ(bytes[0], 0x02);
        EXPECT_EQ(bytes[1], 0x04);
        EXPECT_EQ(bytes[2], 251);
        EXPECT_EQ(bytes.back(), 0x03);
    }
    {
        std::vector<uint8_t> deframed_data;
        bool done_signalled = false;
        bool error_signalled = false;
        auto deframer = poly::com::make_stx_etx_deframer<poly::com::default_unstuffer>([&](poly::com::framed_byte framer) {
            if(framer.byte) {
                deframed_data.push_back(*framer.byte);
            }
            done_signalled = framer.frame_is_done();
            error_signalled = framer.frame_is_error();
        });
        deframer(bytes.begin(), bytes.end());

        ASSERT_EQ(deframed_data.size(), 1);
        EXPECT_FALSE(error_signalled);
        EXPECT_TRUE(done_signalled);
        EXPECT_EQ(deframed_data[0], 0x04);
    }
}

TEST(StxEtxFramer, UnexpectedStx)
{
    std::vector<uint8_t> bytes;
    {
        auto framer = poly::com::make_stx_etx_framer<poly::com::default_stuffer>([&](uint8_t byte) {
            bytes.push_back(byte);
        });

        framer(0x04);
        framer.finish();
        ASSERT_EQ(bytes.size(), 1 + 1 + 1 + 2 + 1); // STX + DLE + byte + CRC + ETX
        EXPECT_EQ(bytes[0], 0x02);
        EXPECT_EQ(bytes[1], 0x04);
        EXPECT_EQ(bytes[2], 251);
        EXPECT_EQ(bytes.back(), 0x03);
    }
    {
        std::vector<std::vector<std::uint8_t>> deframed_data;
        std::vector<uint8_t> current_frame;
        int errors = 0;
        auto deframer = poly::com::make_stx_etx_deframer<poly::com::default_unstuffer>([&](poly::com::framed_byte framer) {
            if(framer.byte) {
                current_frame.push_back(*framer.byte);
            }
            else if(framer.frame_is_done()) {
                deframed_data.push_back(current_frame);
                current_frame.clear();
            }
            else if(framer.frame_is_error()) {
                current_frame.clear();
                errors++;
            }
        });
        auto expected = bytes;

        bytes.insert(bytes.begin(), 0x02);
        bytes.insert(bytes.begin(), 0x56);
        bytes.insert(bytes.begin(), 0x78);
        deframer(bytes.begin(), bytes.end());

        ASSERT_EQ(deframed_data.size(), 1);
        EXPECT_EQ(errors, 1);
        EXPECT_EQ(deframed_data[0].size(), 1);
        EXPECT_EQ(deframed_data[0][0], 0x04);
    }
}

TEST(StxEtxFramer, FnPntr) {
    static std::vector<uint8_t> bytes;
    {
        bytes.clear();

        auto framer = poly::com::stx_etx_framer<poly::com::legacy_stuffer, void (*)(uint8_t)>([](uint8_t byte) {
            bytes.push_back(byte);
        });

        framer(0x04);
        framer(0x02);
        framer.finish();
        ASSERT_EQ(bytes.size(), 1 + 1 + 1 + 1 + 1 + 2 + 1); // STX + DLE + byte + DLE + byte + CRC + ETX
        EXPECT_EQ(bytes[0], 0x02);
        EXPECT_EQ(bytes[1], 0x10);
        EXPECT_EQ(bytes[2], 0x14);
        EXPECT_EQ(bytes.back(), 0x03);
    }
    {
        static std::vector<uint8_t> deframed_bytes;

        deframed_bytes.clear();
        static bool done_signalled = false;
        static bool error_signalled = false;

        done_signalled = false;
        error_signalled = false;

        auto deframer = poly::com::stx_etx_deframer<poly::com::legacy_unstuffer, void(*)(poly::com::framed_byte framer)> (
            [](poly::com::framed_byte framed_byte) {
            if(framed_byte.byte) {
                deframed_bytes.push_back(*framed_byte.byte);
            }
            done_signalled = framed_byte.frame_is_done();
            error_signalled = framed_byte.frame_is_error();
        });
        deframer(bytes.begin(), bytes.end());

        ASSERT_EQ(deframed_bytes.size(), 2);
        EXPECT_FALSE(error_signalled);
        EXPECT_TRUE(done_signalled);
        EXPECT_EQ(deframed_bytes[0], 0x04);
        EXPECT_EQ(deframed_bytes[1], 0x02);

        deframed_bytes.clear();
        done_signalled = false;
        error_signalled = false;

        deframer(bytes.begin(), bytes.end());
        ASSERT_EQ(deframed_bytes.size(), 2);
        EXPECT_FALSE(error_signalled);
        EXPECT_TRUE(done_signalled);
        EXPECT_EQ(deframed_bytes[0], 0x04);
        EXPECT_EQ(deframed_bytes[1], 0x02);
    }
}