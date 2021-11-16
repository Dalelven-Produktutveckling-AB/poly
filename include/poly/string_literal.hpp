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

namespace poly
{
/**
 * @brief A class that can only be constructed by string literals.
 *
 * The only way to construct objects of this type
 * is via the `_str` user-defined literal.
 *
 * The class is copyable and movable, but not default-constructible.
 *
 * ## Example:
 *
 * `"hello world"_str`
 */
class string_literal
{
    const char* str_;
    constexpr explicit string_literal(const char* str): str_(str) {}

    friend constexpr inline string_literal operator "" _str(const char*, std::size_t);
public:
    constexpr string_literal(const string_literal&) noexcept = default;
    constexpr string_literal(string_literal&&) noexcept = default;

    string_literal& operator=(const string_literal&) noexcept = default;
    string_literal& operator=(string_literal&&) noexcept = default;

    /**
     * @brief Get the contained string.
     * @return A pointer to the contained string.
     */
    constexpr inline const char* string() {
        return str_;
    }
};

/**
 * @brief User-defined literal to create `string_literal` instances.
 * @param str The string
 * @param sz Size of the string
 * @return A `string_literal` object pointing to `str`.
 *
 * This operator will only match `"string literals"_str`.
 */
constexpr inline string_literal operator "" _str(const char *str, std::size_t sz) {
    (void)sz;
    return string_literal(str);
}
}

using poly::operator""_str;

#include "macro.hpp"