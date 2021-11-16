/**
* Copyright © 2021 Dalelven Produktutveckling AB
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
* IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
