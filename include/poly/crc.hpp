#pragma once

#include "etl/span.h"

#include <cstdint>

namespace poly::crc
{
struct crc16_ccitt
{
    uint16_t state_ = 0xFFFF;
    crc16_ccitt() = default;
    crc16_ccitt(uint16_t start_state): state_(start_state) {}
    crc16_ccitt operator()(uint8_t data) const
    {
        data ^= state_ & 0xFF;
        data ^= data << 4;

        uint32_t retval = ((((uint32_t)data << 8) | ((state_ & 0xFF00) >> 8))
                             ^ (unsigned char)(data >> 4)
                             ^ ((uint32_t)data << 3));
        return retval & 0x0000'FFFFu;
    }

    uint16_t crc() const {
        return state_;
    }
};
template<class Algorithm>
Algorithm calculate(etl::span<const uint8_t> data, Algorithm crc = Algorithm{})
{
    for(auto b: data) {
        crc = crc(b);
    }
    return crc;
}
}
