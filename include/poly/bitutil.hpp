#pragma once

#include "poly/type_traits.hpp"
#include "poly/result.hpp"
#include "poly/string_literal.hpp"

#include "etl/span.h"

namespace poly::bitutil
{
template<class T, class IterBegin, class IterEnd>
poly::enable_if_t<poly::is_integral_v<T>, T> little_endian_decode_unchecked(IterBegin begin, IterEnd end)
{
    if constexpr(poly::is_unsigned_v<T>)
    {
        (void)end;
        T retval = 0;
        for(size_t i=0; i<sizeof(T); i++)
        {
            T chunk = *begin;
            ++begin;

            chunk <<= i*8u;
            retval += chunk;
        }
        return retval;
    }
    else {
        return static_cast<T>(little_endian_decode_unchecked<poly::make_unsigned_t<T>>(begin, end));
    }
}
template<class T>
poly::enable_if_t<poly::is_integral_v<T>, T> little_endian_decode_unchecked(etl::span<const uint8_t> data)
{
    return poly::bitutil::little_endian_decode_unchecked<T>(data.begin(), data.end());
}

template<class T, class IterBegin, class IterEnd>
poly::enable_if_t<poly::is_integral_v<T>, poly::result<T, poly::string_literal>>
little_endian_decode(IterBegin begin, IterEnd end)
{
    if constexpr(poly::is_unsigned_v<T>)
    {
        T retval = 0;
        for(size_t i=0; i<sizeof(T); i++)
        {
            if(begin == end)
            {
                return poly::error(POLY_ERROR_STR("Not enough data to decode"));
            }
            T chunk = *begin;
            ++begin;

            chunk <<= i*8u;
            retval += chunk;
        }
        return poly::ok(retval);
    }
    else {
        return little_endian_decode<poly::make_unsigned_t<T>>(begin, end)
            .map([](auto usign_value) {
                return static_cast<T>(usign_value);
            });
    }
}

template<class T>
poly::enable_if_t<poly::is_integral_v<T>, poly::result<T, poly::string_literal>>
little_endian_decode(etl::span<const uint8_t> data)
{
    if(data.size() < sizeof(T))
    {
        return poly::error(POLY_ERROR_STR("Not enough data to decode"));
    }
    return poly::ok(little_endian_decode_unchecked<T>(data.begin(), data.end()));
}

template<class T, class Iter>
poly::enable_if_t<poly::is_integral_v<T>> little_endian_encode_unchecked(T value, Iter out)
{
    if constexpr(poly::is_unsigned_v<T> && sizeof(T) == 1)
    {
        *out = value;
        (void)++out;
    }
    else if constexpr(poly::is_unsigned_v<T>)
    {
        for(size_t i=0; i<sizeof(T); i++)
        {
            *out = value & 0x00FFu;
            ++out;
            value >>= 8u;
        }
    }
    else
    {
        return little_endian_encode_unchecked(static_cast<etl::make_unsigned_t<T>>(value), out);
    }
}

template<class T>
poly::enable_if_t<poly::is_integral_v<T>> little_endian_encode_unchecked(T value, etl::span<uint8_t> output)
{
    return little_endian_encode_unchecked(value, output.begin());
}

template<class T, class OutIterBegin, class OutIterEnd>
poly::enable_if_t<poly::is_integral_v<T>, poly::result<OutIterBegin, poly::string_literal>>
little_endian_encode(T value, OutIterBegin begin, OutIterEnd end)
{
    if(begin == end)
    {
        return poly::error(POLY_ERROR_STR("Output area not big enough"));
    }
    else
    {
        if constexpr(poly::is_unsigned_v<T> && sizeof(T) == 1)
        {
            *begin = value;
            return poly::ok(++begin);
        }
        else if constexpr(poly::is_unsigned_v<T>)
        {
            for(size_t i=0; i<sizeof(T); i++)
            {
                if(begin == end)
                {
                    return poly::error(POLY_ERROR_STR("Output area not big enough"));
                }
                *begin = value & 0x00FFu;
                ++begin;
                value >>= 8u;
            }
            return poly::ok(begin);
        }
        else
        {
            return little_endian_encode(static_cast<etl::make_unsigned_t<T>>(value), begin, end);
        }
    }
}


template<class T>
poly::enable_if_t<poly::is_integral_v<T>, poly::result<void, poly::string_literal>>
little_endian_encode(T value, etl::span<uint8_t> output_area) {
    if(output_area.size() < sizeof(T)) {
        return poly::error(POLY_ERROR_STR("Output area not big enough"));
    }

    little_endian_encode_unchecked(value, output_area.begin());
    return poly::ok();
}
}