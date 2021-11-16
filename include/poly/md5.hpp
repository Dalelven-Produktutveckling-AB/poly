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
#include "etl/array.h"
#include "etl/variant.h"

#include <cstdint>

namespace poly::hash
{
namespace detail
{
    struct MD5Context {
        uint32_t buf[4];
        uint32_t bits[2];
        uint8_t in[64];
    };
}

class md5
{
public:
    typedef uint8_t value_type;
    static constexpr size_t digest_size = 16;
    using digest = etl::array<uint8_t, digest_size>;

    md5();
    void operator()(uint8_t data);
    void operator()(etl::span<const uint8_t> buffer);

    /**
     * @brief Resets the MD5 digest operation.
     * 
     */
    void reset();

    /**
     * @brief Updates the MD5 with a new block of data.
     * 
     * @param buffer Data buffer
     * @return True if update, false if already finalized.
     */
    bool update(etl::span<const uint8_t> buffer);

    /**
     * @brief Updates the MD5 with a new data value. For back_inserter support.
     * 
     * @param value Data value
     */
    void push_back(uint8_t data);
    
    /**
     * @brief Ends the MD5 digest operation, returning the digest. To start a new hashing, reset() must be called.
     * 
     * @return digest MD5 digest
     */
    digest finalize();

private:
    etl::variant<detail::MD5Context, digest> context;
};

}
