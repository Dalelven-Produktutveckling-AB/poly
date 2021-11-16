// -*- C++ -*-
//===---------------------------- ratio -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

///////////////////////////////////////
// This file has been modified by
// Dalelven Produktutveckling AB
///////////////////////////////////////

#pragma once

/*
    ratio synopsis

namespace std
{

template <intmax_t N, intmax_t D = 1>
class ratio
{
public:
    static constexpr intmax_t num;
    static constexpr intmax_t den;
    typedef ratio<num, den> type;
};

// ratio arithmetic
template <class R1, class R2> using ratio_add = ...;
template <class R1, class R2> using ratio_subtract = ...;
template <class R1, class R2> using ratio_multiply = ...;
template <class R1, class R2> using ratio_divide = ...;

// ratio comparison
template <class R1, class R2> struct ratio_equal;
template <class R1, class R2> struct ratio_not_equal;
template <class R1, class R2> struct ratio_less;
template <class R1, class R2> struct ratio_less_equal;
template <class R1, class R2> struct ratio_greater;
template <class R1, class R2> struct ratio_greater_equal;

// convenience SI typedefs
typedef ratio<1, 1000000000000000000000000> yocto;  // not supported
typedef ratio<1,    1000000000000000000000> zepto;  // not supported
typedef ratio<1,       1000000000000000000> atto;
typedef ratio<1,          1000000000000000> femto;
typedef ratio<1,             1000000000000> pico;
typedef ratio<1,                1000000000> nano;
typedef ratio<1,                   1000000> micro;
typedef ratio<1,                      1000> milli;
typedef ratio<1,                       100> centi;
typedef ratio<1,                        10> deci;
typedef ratio<                       10, 1> deca;
typedef ratio<                      100, 1> hecto;
typedef ratio<                     1000, 1> kilo;
typedef ratio<                  1000000, 1> mega;
typedef ratio<               1000000000, 1> giga;
typedef ratio<            1000000000000, 1> tera;
typedef ratio<         1000000000000000, 1> peta;
typedef ratio<      1000000000000000000, 1> exa;
typedef ratio<   1000000000000000000000, 1> zetta;  // not supported
typedef ratio<1000000000000000000000000, 1> yotta;  // not supported

  // 20.11.5, ratio comparison
  template <class R1, class R2> inline constexpr bool ratio_equal_v
    = ratio_equal<R1, R2>::value;                                       // C++17
  template <class R1, class R2> inline constexpr bool ratio_not_equal_v
    = ratio_not_equal<R1, R2>::value;                                   // C++17
  template <class R1, class R2> inline constexpr bool ratio_less_v
    = ratio_less<R1, R2>::value;                                        // C++17
  template <class R1, class R2> inline constexpr bool ratio_less_equal_v
    = ratio_less_equal<R1, R2>::value;                                  // C++17
  template <class R1, class R2> inline constexpr bool ratio_greater_v
    = ratio_greater<R1, R2>::value;                                     // C++17
  template <class R1, class R2> inline constexpr bool ratio_greater_equal_v
    = ratio_greater_equal<R1, R2>::value;                               // C++17
}
*/

#include <cstdint>
#include <climits>
#include "type_traits.hpp"


namespace poly {

// static_gcd

namespace detail
{
template <intmax_t Xp, intmax_t Yp>
struct static_gcd
{
    static const intmax_t value = static_gcd<Yp, Xp % Yp>::value;
};

template <intmax_t Xp>
struct static_gcd<Xp, 0>
{
    static const intmax_t value = Xp;
};

template <>
struct static_gcd<0, 0>
{
    static const intmax_t value = 1;
};

// static_lcm

template <intmax_t Xp, intmax_t Yp>
struct static_lcm
{
    static const intmax_t value = Xp / static_gcd<Xp, Yp>::value * Yp;
};

template <intmax_t Xp>
struct static_abs
{
    static const intmax_t value = Xp < 0 ? -Xp : Xp;
};

template <intmax_t Xp>
struct static_sign
{
    static const intmax_t value = Xp == 0 ? 0 : (Xp < 0 ? -1 : 1);
};

template <intmax_t Xp, intmax_t Yp, intmax_t = static_sign<Yp>::value>
class ll_add;

template <intmax_t Xp, intmax_t Yp>
class ll_add<Xp, Yp, 1>
{
    static const intmax_t min = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1)) + 1;
    static const intmax_t max = -min;

    static_assert(Xp <= max - Yp, "overflow in ll_add");
public:
    static const intmax_t value = Xp + Yp;
};

template <intmax_t Xp, intmax_t Yp>
class ll_add<Xp, Yp, 0>
{
public:
    static const intmax_t value = Xp;
};

template <intmax_t Xp, intmax_t Yp>
class ll_add<Xp, Yp, -1>
{
    static const intmax_t min = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1)) + 1;
    static const intmax_t max = -min;

    static_assert(min - Yp <= Xp, "overflow in ll_add");
public:
    static const intmax_t value = Xp + Yp;
};

template <intmax_t Xp, intmax_t Yp, intmax_t = static_sign<Yp>::value>
class ll_sub;

template <intmax_t Xp, intmax_t Yp>
class ll_sub<Xp, Yp, 1>
{
    static const intmax_t min = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1)) + 1;
    static const intmax_t max = -min;

    static_assert(min + Yp <= Xp, "overflow in ll_sub");
public:
    static const intmax_t value = Xp - Yp;
};

template <intmax_t Xp, intmax_t Yp>
class ll_sub<Xp, Yp, 0>
{
public:
    static const intmax_t value = Xp;
};

template <intmax_t Xp, intmax_t Yp>
class ll_sub<Xp, Yp, -1>
{
    static const intmax_t min = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1)) + 1;
    static const intmax_t max = -min;

    static_assert(Xp <= max + Yp, "overflow in ll_sub");
public:
    static const intmax_t value = Xp - Yp;
};

template <intmax_t Xp, intmax_t Yp>
class ll_mul
{
    static const intmax_t nan = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1));
    static const intmax_t min = nan + 1;
    static const intmax_t max = -min;
    static const intmax_t a_x = static_abs<Xp>::value;
    static const intmax_t a_y = static_abs<Yp>::value;

    static_assert(Xp != nan && Yp != nan && a_x <= max / a_y, "overflow in ll_mul");
public:
    static const intmax_t value = Xp * Yp;
};

template <intmax_t Yp>
class ll_mul<0, Yp>
{
public:
    static const intmax_t value = 0;
};

template <intmax_t Xp>
class ll_mul<Xp, 0>
{
public:
    static const intmax_t value = 0;
};

template <>
class ll_mul<0, 0>
{
public:
    static const intmax_t value = 0;
};

// Not actually used but left here in case needed in future maintenance
template <intmax_t Xp, intmax_t Yp>
class ll_div
{
    static const intmax_t nan = (1LL << (sizeof(intmax_t) * CHAR_BIT - 1));
    static const intmax_t min = nan + 1;
    static const intmax_t max = -min;

    static_assert(Xp != nan && Yp != nan && Yp != 0, "overflow in ll_div");
public:
    static const intmax_t value = Xp / Yp;
};
}

using ::poly::detail::static_abs;
using ::poly::detail::static_gcd;
using ::poly::detail::static_lcm;
using ::poly::detail::static_sign;

template <intmax_t Num, intmax_t Den = 1>
class ratio
{
    static_assert(static_abs<Num>::value >= 0, "ratio numerator is out of range");
    static_assert(Den != 0, "ratio divide by 0");
    static_assert(static_abs<Den>::value >  0, "ratio denominator is out of range");
    static constexpr const intmax_t na = static_abs<Num>::value;
    static constexpr const intmax_t da = static_abs<Den>::value;
    static constexpr const intmax_t s = static_sign<Num>::value * static_sign<Den>::value;
    static constexpr const intmax_t gcd = static_gcd<na, da>::value;
public:
    static constexpr const intmax_t num_orig = Num;
    static constexpr const intmax_t den_orig = Den;
    static constexpr const intmax_t num = s * na / gcd;
    static constexpr const intmax_t den = da / gcd;

    typedef ratio<num, den> type;
};

template <intmax_t Num, intmax_t Den>
constexpr const intmax_t ratio<Num, Den>::num;

template <intmax_t Num, intmax_t Den>
constexpr const intmax_t ratio<Num, Den>::den;

template <class _Tp>                    struct is_ratio                     : false_type {};
template <intmax_t Num, intmax_t Den> struct is_ratio<ratio<Num, Den> > : true_type  {};

#ifdef RATIO_LESS_THAN_MICRO
typedef ratio<1LL, 1000000000000000000LL> atto;
typedef ratio<1LL,    1000000000000000LL> femto;
typedef ratio<1LL,       1000000000000LL> pico;
typedef ratio<1LL,          1000000000LL> nano;
#endif
typedef ratio<1LL,             1000000LL> micro;
typedef ratio<1LL,                1000LL> milli;
typedef ratio<1LL,                 100LL> centi;
typedef ratio<1LL,                  10LL> deci;
typedef ratio<                 10LL, 1LL> deca;
typedef ratio<                100LL, 1LL> hecto;
typedef ratio<               1000LL, 1LL> kilo;
typedef ratio<            1000000LL, 1LL> mega;
typedef ratio<         1000000000LL, 1LL> giga;
#ifdef RATIO_GREATER_THAN_GIGA
typedef ratio<      1000000000000LL, 1LL> tera;
typedef ratio<   1000000000000000LL, 1LL> peta;
typedef ratio<1000000000000000000LL, 1LL> exa;
#endif

namespace detail
{
template <class R1, class R2>
struct ratio_multiply
{
private:
    static const intmax_t gcd_n1_d2 = static_gcd<R1::num, R2::den>::value;
    static const intmax_t gcd_d1_n2 = static_gcd<R1::den, R2::num>::value;
public:
    typedef typename ratio
        <
            ll_mul<R1::num / gcd_n1_d2, R2::num / gcd_d1_n2>::value,
            ll_mul<R2::den / gcd_n1_d2, R1::den / gcd_d1_n2>::value
        >::type type;
};
}

template <class R1, class R2> using ratio_multiply
                                    = typename ::poly::detail::ratio_multiply<R1, R2>::type;


namespace detail
{
template <class R1, class R2>
struct ratio_divide
{
private:
    static const intmax_t gcd_n1_n2 = static_gcd<R1::num, R2::num>::value;
    static const intmax_t gcd_d1_d2 = static_gcd<R1::den, R2::den>::value;
public:
    typedef typename ratio
        <
            ll_mul<R1::num / gcd_n1_n2, R2::den / gcd_d1_d2>::value,
            ll_mul<R2::num / gcd_n1_n2, R1::den / gcd_d1_d2>::value
        >::type type;
};
}

template <class R1, class R2> using ratio_divide
                                      = typename ::poly::detail::ratio_divide<R1, R2>::type;


namespace detail
{
template <class R1, class R2>
struct ratio_add
{
private:
    static const intmax_t gcd_n1_n2 = static_gcd<R1::num, R2::num>::value;
    static const intmax_t gcd_d1_d2 = static_gcd<R1::den, R2::den>::value;
public:
    typedef typename ratio_multiply
        <
            ratio<gcd_n1_n2, R1::den / gcd_d1_d2>,
            ratio
            <
                ll_add
                <
                    ll_mul<R1::num / gcd_n1_n2, R2::den / gcd_d1_d2>::value,
                    ll_mul<R2::num / gcd_n1_n2, R1::den / gcd_d1_d2>::value
                >::value,
                R2::den
            >
        >::type type;
};
}

template <class R1, class R2> using ratio_add
                                         = typename detail::ratio_add<R1, R2>::type;

namespace detail
{
template <class R1, class R2>
struct ratio_subtract
{
private:
    static const intmax_t gcd_n1_n2 = static_gcd<R1::num, R2::num>::value;
    static const intmax_t gcd_d1_d2 = static_gcd<R1::den, R2::den>::value;
public:
    typedef typename ratio_multiply
        <
            ratio<gcd_n1_n2, R1::den / gcd_d1_d2>,
            ratio
            <
                ll_sub
                <
                    ll_mul<R1::num / gcd_n1_n2, R2::den / gcd_d1_d2>::value,
                    ll_mul<R2::num / gcd_n1_n2, R1::den / gcd_d1_d2>::value
                >::value,
                R2::den
            >
        >::type type;
};
}

template <class R1, class R2> using ratio_subtract
                                    = typename detail::ratio_subtract<R1, R2>::type;

// ratio_equal

template <class R1, class R2>
struct ratio_equal
    : public poly::bool_constant<R1::num == R2::num && R1::den == R2::den> {};

template <class R1, class R2>
struct ratio_not_equal
    : public poly::bool_constant<!ratio_equal<R1, R2>::value> {};

// ratio_less

namespace detail
{
template <class R1, class R2, bool Odd = false,
          intmax_t _Q1 = R1::num / R1::den, intmax_t _M1 = R1::num % R1::den,
          intmax_t _Q2 = R2::num / R2::den, intmax_t _M2 = R2::num % R2::den>
struct ratio_less1
{
    static const bool value = Odd ? _Q2 < _Q1 : _Q1 < _Q2;
};

template <class R1, class R2, bool Odd, intmax_t _Qp>
struct ratio_less1<R1, R2, Odd, _Qp, 0, _Qp, 0>
{
    static const bool value = false;
};

template <class R1, class R2, bool Odd, intmax_t _Qp, intmax_t _M2>
struct ratio_less1<R1, R2, Odd, _Qp, 0, _Qp, _M2>
{
    static const bool value = !Odd;
};

template <class R1, class R2, bool Odd, intmax_t _Qp, intmax_t _M1>
struct ratio_less1<R1, R2, Odd, _Qp, _M1, _Qp, 0>
{
    static const bool value = Odd;
};

template <class R1, class R2, bool Odd, intmax_t _Qp, intmax_t _M1,
                                                        intmax_t _M2>
struct ratio_less1<R1, R2, Odd, _Qp, _M1, _Qp, _M2>
{
    static const bool value = ratio_less1<ratio<R1::den, _M1>,
                                            ratio<R2::den, _M2>, !Odd>::value;
};

template <class R1, class R2, intmax_t _S1 = static_sign<R1::num>::value,
                                intmax_t _S2 = static_sign<R2::num>::value>
struct ratio_less
{
    static const bool value = _S1 < _S2;
};

template <class R1, class R2>
struct ratio_less<R1, R2, 1LL, 1LL>
{
    static const bool value = ratio_less1<R1, R2>::value;
};

template <class R1, class R2>
struct ratio_less<R1, R2, -1LL, -1LL>
{
    static const bool value = ratio_less1<ratio<-R2::num, R2::den>, ratio<-R1::num, R1::den> >::value;
};
}

template <class R1, class R2>
struct  ratio_less
    : public poly::bool_constant<detail::ratio_less<R1, R2>::value> {};

template <class R1, class R2>
struct  ratio_less_equal
    : public poly::bool_constant<!ratio_less<R2, R1>::value> {};

template <class R1, class R2>
struct  ratio_greater
    : public poly::bool_constant<ratio_less<R2, R1>::value> {};

template <class R1, class R2>
struct  ratio_greater_equal
    : public poly::bool_constant<!ratio_less<R1, R2>::value> {};

namespace detail
{
template <class R1, class R2>
struct ratio_gcd
{
    typedef ratio<static_gcd<R1::num, R2::num>::value,
                  static_lcm<R1::den, R2::den>::value> type;
};
}

template <class R1, class R2>
inline constexpr bool ratio_equal_v
    = ratio_equal<R1, R2>::value;

template <class R1, class R2>
inline constexpr bool ratio_not_equal_v
    = ratio_not_equal<R1, R2>::value;

template <class R1, class R2>
inline constexpr bool ratio_less_v
    = ratio_less<R1, R2>::value;

template <class R1, class R2>
inline constexpr bool ratio_less_equal_v
    = ratio_less_equal<R1, R2>::value;

template <class R1, class R2>
inline constexpr bool ratio_greater_v
    = ratio_greater<R1, R2>::value;

template <class R1, class R2>
inline constexpr bool ratio_greater_equal_v
    = ratio_greater_equal<R1, R2>::value;

}