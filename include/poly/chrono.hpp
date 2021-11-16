// -*- C++ -*-
//===---------------------------- chrono ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


#pragma once

#include <ctime>
#include <cstdint>
#include "limits.hpp"
#include "ratio.hpp"

namespace poly
{
namespace chrono
{

template <class Rep, class Period = ratio<1> > class  duration;

template <class Tp>
struct detail_is_duration : false_type {};

template <class Rep, class Period>
struct detail_is_duration<duration<Rep, Period> > : true_type  {};

template <class Rep, class Period>
struct detail_is_duration<const duration<Rep, Period> > : true_type  {};

template <class Rep, class Period>
struct detail_is_duration<volatile duration<Rep, Period> > : true_type  {};

template <class Rep, class Period>
struct detail_is_duration<const volatile duration<Rep, Period> > : true_type  {};

} // chrono

template <class Rep1, class Period1, class Rep2, class Period2>
struct  common_type<chrono::duration<Rep1, Period1>,
                                         chrono::duration<Rep2, Period2> >
{
    typedef poly::chrono::duration<typename common_type<Rep1, Rep2>::type,
                             typename poly::detail::ratio_gcd<Period1, Period2>::type> type;
};

namespace chrono {

// duration_cast

template <class FromDuration, class ToDuration,
          class Period = typename ratio_divide<typename FromDuration::period, typename ToDuration::period>::type,
          bool = Period::num == 1,
          bool = Period::den == 1>
struct detail_duration_cast;

template <class FromDuration, class ToDuration, class Period>
struct detail_duration_cast<FromDuration, ToDuration, Period, true, true>
{
    inline constexpr
    ToDuration operator()(const FromDuration& fd) const
    {
        return ToDuration(static_cast<typename ToDuration::rep>(fd.count()));
    }
};

template <class FromDuration, class ToDuration, class Period>
struct detail_duration_cast<FromDuration, ToDuration, Period, true, false>
{
    inline constexpr
    ToDuration operator()(const FromDuration& fd) const
    {
        typedef typename common_type<typename ToDuration::rep, typename FromDuration::rep, intmax_t>::type CommonType;
        return ToDuration(static_cast<typename ToDuration::rep>(
                           static_cast<CommonType>(fd.count()) / static_cast<CommonType>(Period::den)));
    }
};

template <class FromDuration, class ToDuration, class Period>
struct detail_duration_cast<FromDuration, ToDuration, Period, false, true>
{
    inline constexpr
    ToDuration operator()(const FromDuration& fd) const
    {
        typedef typename common_type<typename ToDuration::rep, typename FromDuration::rep, intmax_t>::type CommonType;
        return ToDuration(static_cast<typename ToDuration::rep>(
                           static_cast<CommonType>(fd.count()) * static_cast<CommonType>(Period::num)));
    }
};

template <class FromDuration, class ToDuration, class Period>
struct detail_duration_cast<FromDuration, ToDuration, Period, false, false>
{
    inline constexpr
    ToDuration operator()(const FromDuration& fd) const
    {
        typedef typename common_type<typename ToDuration::rep, typename FromDuration::rep, intmax_t>::type CommonType;
        return ToDuration(static_cast<typename ToDuration::rep>(
                           static_cast<CommonType>(fd.count()) * static_cast<CommonType>(Period::num)
                                                          / static_cast<CommonType>(Period::den)));
    }
};

template <class ToDuration, class Rep, class Period>
inline
constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    ToDuration
>::type
duration_cast(const duration<Rep, Period>& fd)
{
    return detail_duration_cast<duration<Rep, Period>, ToDuration>()(fd);
}

template <class Rep>
struct  treat_as_floating_point : is_floating_point<Rep> {};

template <class Rep>
inline constexpr bool treat_as_floating_point_v
    = treat_as_floating_point<Rep>::value;

template <class Rep>
struct  duration_values
{
public:
    inline static constexpr Rep zero() noexcept {return Rep(0);}
    inline static constexpr Rep max()  noexcept {return numeric_limits<Rep>::max();}
    inline static constexpr Rep min()  noexcept {return numeric_limits<Rep>::lowest();}
};

template <class ToDuration, class Rep, class Period>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    ToDuration
>::type
floor(const duration<Rep, Period>& d)
{
    ToDuration t = duration_cast<ToDuration>(d);
    if (t > d)
        t = t - ToDuration{1};
    return t;
}

template <class ToDuration, class Rep, class Period>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    ToDuration
>::type
ceil(const duration<Rep, Period>& d)
{
    ToDuration t = duration_cast<ToDuration>(d);
    if (t < d)
        t = t + ToDuration{1};
    return t;
}

template <class ToDuration, class Rep, class Period>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    ToDuration
>::type
round(const duration<Rep, Period>& d)
{
    ToDuration lower = floor<ToDuration>(d);
    ToDuration upper = lower + ToDuration{1};
    auto lowerDiff = d - lower;
    auto upperDiff = upper - d;
    if (lowerDiff < upperDiff)
        return lower;
    if (lowerDiff > upperDiff)
        return upper;
    return lower.count() & 1 ? upper : lower;
}

// duration

template <class Rep, class Period>
class  duration
{
    static_assert(!detail_is_duration<Rep>::value, "A duration representation can not be a duration");
    static_assert(poly::is_ratio<Period>::value, "Second template parameter of duration must be a poly::ratio");
    static_assert(Period::num > 0, "duration period must be positive");

    template <class R1, class R2>
    struct detail_no_overflow
    {
    private:
        static const intmax_t gcd_n1_n2 = detail::static_gcd<R1::num, R2::num>::value;
        static const intmax_t gcd_d1_d2 = detail::static_gcd<R1::den, R2::den>::value;
        static const intmax_t n1 = R1::num / gcd_n1_n2;
        static const intmax_t d1 = R1::den / gcd_d1_d2;
        static const intmax_t n2 = R2::num / gcd_n1_n2;
        static const intmax_t d2 = R2::den / gcd_d1_d2;
        static const intmax_t max = -((intmax_t(1) << (sizeof(intmax_t) * CHAR_BIT - 1)) + 1);

        template <intmax_t Xp, intmax_t Yp, bool overflow>
        struct detail_mul    // overflow == false
        {
            static const intmax_t value = Xp * Yp;
        };

        template <intmax_t Xp, intmax_t Yp>
        struct detail_mul<Xp, Yp, true>
        {
            static const intmax_t value = 1;
        };

    public:
        static const bool value = (n1 <= max / d2) && (n2 <= max / d1);
        typedef ratio<detail_mul<n1, d2, !value>::value,
                      detail_mul<n2, d1, !value>::value> type;
    };

public:
    typedef Rep rep;
    typedef typename Period::type period;
private:
    rep rep_;
public:

    inline constexpr
    duration() = default;

    template <class Rep2>
        inline constexpr
        explicit duration(const Rep2& r,
            typename enable_if
            <
               is_convertible<Rep2, rep>::value &&
               (treat_as_floating_point<rep>::value ||
               !treat_as_floating_point<Rep2>::value)
            >::type* = 0)
                : rep_(r) {}

    // conversions
    template <class Rep2, class Period2>
        inline constexpr
        duration(const duration<Rep2, Period2>& d,
            typename enable_if
            <
                detail_no_overflow<Period2, period>::value && (
                treat_as_floating_point<rep>::value ||
                (detail_no_overflow<Period2, period>::type::den == 1 &&
                 !treat_as_floating_point<Rep2>::value))
            >::type* = 0)
                : rep_(::poly::chrono::duration_cast<duration>(d).count()) {}

    // observer

    inline constexpr rep count() const {return rep_;}

    // arithmetic

    inline constexpr typename common_type<duration>::type operator+() const {return typename common_type<duration>::type(*this);}
    inline constexpr typename common_type<duration>::type operator-() const {return typename common_type<duration>::type(-rep_);}
    inline constexpr duration& operator++()      {++rep_; return *this;}
    inline constexpr duration  operator++(int)   {return duration(rep_++);}
    inline constexpr duration& operator--()      {--rep_; return *this;}
    inline constexpr duration  operator--(int)   {return duration(rep_--);}

    inline constexpr duration& operator+=(const duration& d) {rep_ += d.count(); return *this;}
    inline constexpr duration& operator-=(const duration& d) {rep_ -= d.count(); return *this;}

    inline constexpr duration& operator*=(const rep& rhs) {rep_ *= rhs; return *this;}
    inline constexpr duration& operator/=(const rep& rhs) {rep_ /= rhs; return *this;}
    inline constexpr duration& operator%=(const rep& rhs) {rep_ %= rhs; return *this;}
    inline constexpr duration& operator%=(const duration& rhs) {rep_ %= rhs.count(); return *this;}

    // special values

    inline static constexpr duration zero() noexcept {return duration(duration_values<rep>::zero());}
    inline static constexpr duration min()  noexcept {return duration(duration_values<rep>::min());}
    inline static constexpr duration max()  noexcept {return duration(duration_values<rep>::max());}
};

typedef duration<std::int64_t,        micro> microseconds;
typedef duration<std::int64_t,        milli> milliseconds;
typedef duration<std::int64_t, ratio<1>> seconds;
typedef duration<std::int32_t, ratio<60> > minutes;
typedef duration<std::int32_t, ratio<3600> > hours;
typedef duration<std::int32_t, ratio_multiply<ratio<24>, hours::period>>         days;
typedef duration<std::int32_t, ratio_multiply<ratio<7>,   days::period>>         weeks;
typedef duration<std::int32_t, ratio_multiply<ratio<146097, 400>, days::period>> years;
typedef duration<std::int32_t, ratio_divide<years::period, ratio<12>>>           months;
// Duration ==

template <class LhsDuration, class RhsDuration>
struct duration_eq
{
    inline constexpr
    bool operator()(const LhsDuration& lhs, const RhsDuration& rhs) const
        {
            typedef typename common_type<LhsDuration, RhsDuration>::type CommonType;
            return CommonType(lhs).count() == CommonType(rhs).count();
        }
};

template <class LhsDuration>
struct duration_eq<LhsDuration, LhsDuration>
{
    inline constexpr
    bool operator()(const LhsDuration& lhs, const LhsDuration& rhs) const
        {return lhs.count() == rhs.count();}
};

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator==(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return duration_eq<duration<Rep1, Period1>, duration<Rep2, Period2> >()(lhs, rhs);
}

// Duration !=

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator!=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return !(lhs == rhs);
}

// Duration <

template <class LhsDuration, class RhsDuration>
struct duration_lt
{
    inline constexpr
    bool operator()(const LhsDuration& lhs, const RhsDuration& rhs) const
        {
            typedef typename common_type<LhsDuration, RhsDuration>::type CommonType;
            return CommonType(lhs).count() < CommonType(rhs).count();
        }
};

template <class LhsDuration>
struct duration_lt<LhsDuration, LhsDuration>
{
    inline constexpr
    bool operator()(const LhsDuration& lhs, const LhsDuration& rhs) const
        {return lhs.count() < rhs.count();}
};

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator< (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return duration_lt<duration<Rep1, Period1>, duration<Rep2, Period2> >()(lhs, rhs);
}

// Duration >

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator> (const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return rhs < lhs;
}

// Duration <=

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator<=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return !(rhs < lhs);
}

// Duration >=

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
bool
operator>=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    return !(lhs < rhs);
}

// Duration +

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type
operator+(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type CommonTypeD;
    return CommonTypeD(CommonTypeD(lhs).count() + CommonTypeD(rhs).count());
}

// Duration -

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type
operator-(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type CommonTypeD;
    return CommonTypeD(CommonTypeD(lhs).count() - CommonTypeD(rhs).count());
}

// Duration *

template <class Rep1, class Period, class Rep2>
inline
constexpr
typename enable_if
<
    is_convertible<Rep2, typename common_type<Rep1, Rep2>::type>::value,
    duration<typename common_type<Rep1, Rep2>::type, Period>
>::type
operator*(const duration<Rep1, Period>& d, const Rep2& s)
{
    typedef typename common_type<Rep1, Rep2>::type CommonTypeRep;
    typedef duration<CommonTypeRep, Period> CommonTypeD;
    return CommonTypeD(CommonTypeD(d).count() * static_cast<CommonTypeRep>(s));
}

template <class Rep1, class Period, class Rep2>
inline
constexpr
typename enable_if
<
    is_convertible<Rep1, typename common_type<Rep1, Rep2>::type>::value,
    duration<typename common_type<Rep1, Rep2>::type, Period>
>::type
operator*(const Rep1& s, const duration<Rep2, Period>& d)
{
    return d * s;
}

// Duration /

template <class Rep1, class Period, class Rep2>
inline
constexpr
typename enable_if
<
    !detail_is_duration<Rep2>::value &&
      is_convertible<Rep2, typename common_type<Rep1, Rep2>::type>::value,
    duration<typename common_type<Rep1, Rep2>::type, Period>
>::type
operator/(const duration<Rep1, Period>& d, const Rep2& s)
{
    typedef typename common_type<Rep1, Rep2>::type CommonTypeRep;
    typedef duration<CommonTypeRep, Period> CommonTypeD;
    return CommonTypeD(CommonTypeD(d).count() / static_cast<CommonTypeRep>(s));
}

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
typename common_type<Rep1, Rep2>::type
operator/(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type CommonType;
    return CommonType(lhs).count() / CommonType(rhs).count();
}

// Duration %

template <class Rep1, class Period, class Rep2>
inline
constexpr
typename enable_if
<
    !detail_is_duration<Rep2>::value &&
      is_convertible<Rep2, typename common_type<Rep1, Rep2>::type>::value,
    duration<typename common_type<Rep1, Rep2>::type, Period>
>::type
operator%(const duration<Rep1, Period>& d, const Rep2& s)
{
    typedef typename common_type<Rep1, Rep2>::type CommonTypeRep;
    typedef duration<CommonTypeRep, Period> CommonTypeD;
    return CommonTypeD(CommonTypeD(d).count() % static_cast<CommonTypeRep>(s));
}

template <class Rep1, class Period1, class Rep2, class Period2>
inline
constexpr
typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type
operator%(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef typename common_type<Rep1, Rep2>::type CommonTypeRep;
    typedef typename common_type<duration<Rep1, Period1>, duration<Rep2, Period2> >::type CommonTypeD;
    return CommonTypeD(static_cast<CommonTypeRep>(CommonTypeD(lhs).count()) % static_cast<CommonTypeRep>(CommonTypeD(rhs).count()));
}

//////////////////////////////////////////////////////////
///////////////////// time_point /////////////////////////
//////////////////////////////////////////////////////////

template <class ClockT, class DurationT = typename ClockT::duration>
class  time_point
{
    static_assert(detail_is_duration<DurationT>::value,
                  "Second template parameter of time_point must be a poly::chrono::duration");
public:
    typedef ClockT                    clock;
    typedef DurationT                 duration;
    typedef typename duration::rep    rep;
    typedef typename duration::period period;
private:
    duration d_;

public:
    inline constexpr time_point() : d_(duration::zero()) {}
    inline constexpr explicit time_point(const duration& d) : d_(d) {}

    // conversions
    template <class DurationT2>
    inline constexpr
    time_point(const time_point<clock, DurationT2>& t,
        typename enable_if
        <
            is_convertible<DurationT2, duration>::value
        >::type* = 0)
            : d_(t.time_since_epoch()) {}

    // observer

    inline constexpr duration time_since_epoch() const {return d_;}

    // arithmetic

    inline constexpr time_point& operator+=(const duration& d) {d_ += d; return *this;}
    inline constexpr time_point& operator-=(const duration& d) {d_ -= d; return *this;}

    // special values

    inline static constexpr time_point min() noexcept {return time_point(duration::min());}
    inline static constexpr time_point max() noexcept {return time_point(duration::max());}
};

} // chrono

template <class ClockT, class DurationT1, class DurationT2>
struct  common_type<poly::chrono::time_point<ClockT, DurationT1>,
                                         chrono::time_point<ClockT, DurationT2> >
{
    typedef poly::chrono::time_point<ClockT, typename common_type<DurationT1, DurationT2>::type> type;
};

namespace chrono {

template <class ToDuration, class ClockT, class DurationT>
inline constexpr
time_point<ClockT, ToDuration>
time_point_cast(const time_point<ClockT, DurationT>& t)
{
    return time_point<ClockT, ToDuration>(poly::chrono::duration_cast<ToDuration>(t.time_since_epoch()));
}

template <class ToDuration, class ClockT, class DurationT>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    time_point<ClockT, ToDuration>
>::type
floor(const time_point<ClockT, DurationT>& t)
{
    return time_point<ClockT, ToDuration>{floor<ToDuration>(t.time_since_epoch())};
}

template <class ToDuration, class ClockT, class DurationT>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    time_point<ClockT, ToDuration>
>::type
ceil(const time_point<ClockT, DurationT>& t)
{
    return time_point<ClockT, ToDuration>{ceil<ToDuration>(t.time_since_epoch())};
}

template <class ToDuration, class ClockT, class DurationT>
inline constexpr
typename enable_if
<
    detail_is_duration<ToDuration>::value,
    time_point<ClockT, ToDuration>
>::type
round(const time_point<ClockT, DurationT>& t)
{
    return time_point<ClockT, ToDuration>{round<ToDuration>(t.time_since_epoch())};
}

template <class Rep, class Period>
inline constexpr
typename enable_if
<
    numeric_limits<Rep>::is_signed,
    duration<Rep, Period>
>::type
abs(duration<Rep, Period> d)
{
    return d >= d.zero() ? +d : -d;
}

// time_point ==

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator==(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return lhs.time_since_epoch() == rhs.time_since_epoch();
}

// time_point !=

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator!=(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return !(lhs == rhs);
}

// time_point <

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator<(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return lhs.time_since_epoch() < rhs.time_since_epoch();
}

// time_point >

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator>(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return rhs < lhs;
}

// time_point <=

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator<=(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return !(rhs < lhs);
}

// time_point >=

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
bool
operator>=(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return !(lhs < rhs);
}

// time_point operator+(time_point x, duration y);

template <class ClockT, class DurationT1, class Rep2, class Period2>
inline constexpr
time_point<ClockT, typename common_type<DurationT1, duration<Rep2, Period2> >::type>
operator+(const time_point<ClockT, DurationT1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef time_point<ClockT, typename common_type<DurationT1, duration<Rep2, Period2> >::type> _Tr;
    return _Tr (lhs.time_since_epoch() + rhs);
}

// time_point operator+(duration x, time_point y);

template <class Rep1, class Period1, class ClockT, class DurationT2>
inline constexpr
time_point<ClockT, typename common_type<duration<Rep1, Period1>, DurationT2>::type>
operator+(const duration<Rep1, Period1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return rhs + lhs;
}

// time_point operator-(time_point x, duration y);

template <class ClockT, class DurationT1, class Rep2, class Period2>
inline constexpr
time_point<ClockT, typename common_type<DurationT1, duration<Rep2, Period2> >::type>
operator-(const time_point<ClockT, DurationT1>& lhs, const duration<Rep2, Period2>& rhs)
{
    typedef time_point<ClockT, typename common_type<DurationT1, duration<Rep2, Period2> >::type> _Ret;
    return _Ret(lhs.time_since_epoch() -rhs);
}

// duration operator-(time_point x, time_point y);

template <class ClockT, class DurationT1, class DurationT2>
inline constexpr
typename common_type<DurationT1, DurationT2>::type
operator-(const time_point<ClockT, DurationT1>& lhs, const time_point<ClockT, DurationT2>& rhs)
{
    return lhs.time_since_epoch() - rhs.time_since_epoch();
}

///////////////////////////////////////////////////////
////// Clocks /////////////////////////////////////////
///////////////////////////////////////////////////////

/**
 * @brief system_clock
 *
 * Epoch is always 1970-01-01 00:00:00UTC not counting leap seconds, also known as Unix epoch.
 */
class system_clock
{
public:
    typedef microseconds                     duration;
    typedef duration::rep                    rep;
    typedef duration::period                 period;
    typedef chrono::time_point<system_clock> time_point;
    static constexpr bool is_steady =            false; // constexpr in C++14
    // Now is not applicable!
    //static time_point now() noexcept;
    static time_t     to_time_t  (const time_point& t) noexcept
    {
        return std::time_t(static_cast<time_t>(duration_cast<seconds>(t.time_since_epoch()).count()));
    }
    static time_point from_time_t(time_t t) noexcept
    {
        return time_point(seconds(static_cast<rep>(t)));
    }
};

template<class Duration>
using sys_time = time_point<system_clock, Duration>;
using sys_milliseconds = sys_time<milliseconds>;
using sys_seconds = sys_time<seconds>;
using sys_days = sys_time<days>;

struct last_spec { explicit last_spec() = default; };

class day {
private:
    unsigned char d;
public:
    day() = default;
    explicit inline constexpr day(unsigned val) noexcept : d(static_cast<unsigned char>(val)) {}
    inline constexpr day& operator++()    noexcept { ++d; return *this; }
    inline constexpr day  operator++(int) noexcept { day tmp = *this; ++(*this); return tmp; }
    inline constexpr day& operator--()    noexcept { --d; return *this; }
    inline constexpr day  operator--(int) noexcept { day tmp = *this; --(*this); return tmp; }
           constexpr day& operator+=(const days& dd) noexcept;
           constexpr day& operator-=(const days& dd) noexcept;
    explicit inline constexpr operator unsigned() const noexcept { return d; }
    inline constexpr bool ok() const noexcept { return d >= 1 && d <= 31; }
  };


inline constexpr
bool operator==(const day& lhs, const day& rhs) noexcept
{ return static_cast<unsigned>(lhs) == static_cast<unsigned>(rhs); }

inline constexpr
bool operator!=(const day& lhs, const day& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const day& lhs, const day& rhs) noexcept
{ return static_cast<unsigned>(lhs) <  static_cast<unsigned>(rhs); }

inline constexpr
bool operator> (const day& lhs, const day& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const day& lhs, const day& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const day& lhs, const day& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr
day operator+ (const day& lhs, const days& rhs) noexcept
{ return day(static_cast<unsigned>(lhs) + rhs.count()); }

inline constexpr
day operator+ (const days& lhs, const day& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
day operator- (const day& lhs, const days& rhs) noexcept
{ return lhs + -rhs; }

inline constexpr
days operator-(const day& lhs, const day& rhs) noexcept
{ return days(static_cast<int>(static_cast<unsigned>(lhs)) -
              static_cast<int>(static_cast<unsigned>(rhs))); }

inline constexpr day& day::operator+=(const days& dd) noexcept
{ *this = *this + dd; return *this; }

inline constexpr day& day::operator-=(const days& dd) noexcept
{ *this = *this - dd; return *this; }


class month {
private:
    unsigned char m_;
public:
    month() = default;
    explicit inline constexpr month(unsigned val) noexcept : m_(static_cast<unsigned char>(val)) {}
    inline constexpr month& operator++()    noexcept { ++m_; return *this; }
    inline constexpr month  operator++(int) noexcept { month tmp = *this; ++(*this); return tmp; }
    inline constexpr month& operator--()    noexcept { --m_; return *this; }
    inline constexpr month  operator--(int) noexcept { month tmp = *this; --(*this); return tmp; }
           constexpr month& operator+=(const months& m_1) noexcept;
           constexpr month& operator-=(const months& m_1) noexcept;
    explicit inline constexpr operator unsigned() const noexcept { return m_; }
    inline constexpr bool ok() const noexcept { return m_ >= 1 && m_ <= 12; }
};


inline constexpr
bool operator==(const month& lhs, const month& rhs) noexcept
{ return static_cast<unsigned>(lhs) == static_cast<unsigned>(rhs); }

inline constexpr
bool operator!=(const month& lhs, const month& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const month& lhs, const month& rhs) noexcept
{ return static_cast<unsigned>(lhs)  < static_cast<unsigned>(rhs); }

inline constexpr
bool operator> (const month& lhs, const month& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const month& lhs, const month& rhs) noexcept
{ return !(rhs < lhs); }

inline constexpr
bool operator>=(const month& lhs, const month& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr
month operator+ (const month& lhs, const months& rhs) noexcept
{
    auto const m_u = static_cast<long long>(static_cast<unsigned>(lhs)) + (rhs.count() - 1);
    auto const yr_r = (m_u >= 0 ? m_u : m_u - 11) / 12;
    return month{static_cast<unsigned>(m_u - yr_r * 12 + 1)};
}

inline constexpr
month operator+ (const months& lhs, const month& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
month operator- (const month& lhs, const months& rhs) noexcept
{ return lhs + -rhs; }

inline constexpr
months operator-(const month& lhs, const month& rhs) noexcept
{
    auto const dm = static_cast<unsigned>(lhs) - static_cast<unsigned>(rhs);
    return months(dm <= 11 ? dm : dm + 12);
}

inline constexpr month& month::operator+=(const months& dm) noexcept
{ *this = *this + dm; return *this; }

inline constexpr month& month::operator-=(const months& dm) noexcept
{ *this = *this - dm; return *this; }


class year {
private:
    short yr_;
public:
    year() = default;
    explicit inline constexpr year(int val) noexcept : yr_(static_cast<short>(val)) {}

    inline constexpr year& operator++()    noexcept { ++yr_; return *this; }
    inline constexpr year  operator++(int) noexcept { year tmp = *this; ++(*this); return tmp; }
    inline constexpr year& operator--()    noexcept { --yr_; return *this; }
    inline constexpr year  operator--(int) noexcept { year tmp = *this; --(*this); return tmp; }
           constexpr year& operator+=(const years& dy) noexcept;
           constexpr year& operator-=(const years& dy) noexcept;
    inline constexpr year operator+() const noexcept { return *this; }
    inline constexpr year operator-() const noexcept { return year{-yr_}; }

    inline constexpr bool is_leap() const noexcept { return yr_ % 4 == 0 && (yr_ % 100 != 0 || yr_ % 400 == 0); }
    explicit inline constexpr operator int() const noexcept { return yr_; }
           constexpr bool ok() const noexcept;
    static inline constexpr year min() noexcept { return year{-32767}; }
    static inline constexpr year max() noexcept { return year{ 32767}; }
};


inline constexpr
bool operator==(const year& lhs, const year& rhs) noexcept
{ return static_cast<int>(lhs) == static_cast<int>(rhs); }

inline constexpr
bool operator!=(const year& lhs, const year& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const year& lhs, const year& rhs) noexcept
{ return static_cast<int>(lhs)  < static_cast<int>(rhs); }

inline constexpr
bool operator> (const year& lhs, const year& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const year& lhs, const year& rhs) noexcept
{ return !(rhs < lhs); }

inline constexpr
bool operator>=(const year& lhs, const year& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr
year operator+ (const year& lhs, const years& rhs) noexcept
{ return year(static_cast<int>(lhs) + rhs.count()); }

inline constexpr
year operator+ (const years& lhs, const year& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
year operator- (const year& lhs, const years& rhs) noexcept
{ return lhs + -rhs; }

inline constexpr
years operator-(const year& lhs, const year& rhs) noexcept
{ return years{static_cast<int>(lhs) - static_cast<int>(rhs)}; }


inline constexpr year& year::operator+=(const years& dy) noexcept
{ *this = *this + dy; return *this; }

inline constexpr year& year::operator-=(const years& dy) noexcept
{ *this = *this - dy; return *this; }

inline constexpr bool year::ok() const noexcept
{ return static_cast<int>(min()) <= yr_ && yr_ <= static_cast<int>(max()); }

class weekday_indexed;
class weekday_last;

class weekday {
private:
    unsigned char wd_;
public:
  weekday() = default;
  inline explicit constexpr weekday(unsigned val) noexcept : wd_(static_cast<unsigned char>(val == 7 ? 0 : val)) {}

  inline constexpr          weekday(const sys_days& sysd) noexcept
  : wd_(detail_weekday_from_days(sysd.time_since_epoch().count())) {}

  inline constexpr weekday& operator++()    noexcept { wd_ = (wd_ == 6 ? 0 : wd_ + 1); return *this; }
  inline constexpr weekday  operator++(int) noexcept { weekday tmp = *this; ++(*this); return tmp; }
  inline constexpr weekday& operator--()    noexcept { wd_ = (wd_ == 0 ? 6 : wd_ - 1); return *this; }
  inline constexpr weekday  operator--(int) noexcept { weekday tmp = *this; --(*this); return tmp; }
         constexpr weekday& operator+=(const days& dd) noexcept;
         constexpr weekday& operator-=(const days& dd) noexcept;
  inline constexpr unsigned c_encoding()   const noexcept { return wd_; }
  inline constexpr unsigned iso_encoding() const noexcept { return wd_ == 0u ? 7 : wd_; }
  inline constexpr bool ok() const noexcept { return wd_ <= 6; }
         constexpr weekday_indexed operator[](unsigned index) const noexcept;
         constexpr weekday_last    operator[](last_spec) const noexcept;

  // TODO: Make private?
  static constexpr unsigned char detail_weekday_from_days(int days) noexcept;
};


// https://howardhinnant.github.io/date_algorithms.html#weekday_from_days
inline constexpr
unsigned char weekday::detail_weekday_from_days(int days) noexcept
{
    return static_cast<unsigned char>(
              static_cast<unsigned>(days >= -4 ? (days+4) % 7 : (days+5) % 7 + 6)
           );
}

inline constexpr
bool operator==(const weekday& lhs, const weekday& rhs) noexcept
{ return lhs.c_encoding() == rhs.c_encoding(); }

inline constexpr
bool operator!=(const weekday& lhs, const weekday& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const weekday& lhs, const weekday& rhs) noexcept
{ return lhs.c_encoding() < rhs.c_encoding(); }

inline constexpr
bool operator> (const weekday& lhs, const weekday& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const weekday& lhs, const weekday& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const weekday& lhs, const weekday& rhs) noexcept
{ return !(lhs < rhs); }

constexpr weekday operator+(const weekday& lhs, const days& rhs) noexcept
{
    auto const m_u = static_cast<long long>(lhs.c_encoding()) + rhs.count();
    auto const yr_r = (m_u >= 0 ? m_u : m_u - 6) / 7;
    return weekday{static_cast<unsigned>(m_u - yr_r * 7)};
}

constexpr weekday operator+(const days& lhs, const weekday& rhs) noexcept
{ return rhs + lhs; }

constexpr weekday operator-(const weekday& lhs, const days& rhs) noexcept
{ return lhs + -rhs; }

constexpr days operator-(const weekday& lhs, const weekday& rhs) noexcept
{
    const int wd_u = lhs.c_encoding() - rhs.c_encoding();
    const int _wk = (wd_u >= 0 ? wd_u : wd_u-6) / 7;
    return days{wd_u - _wk * 7};
}

inline constexpr weekday& weekday::operator+=(const days& dd) noexcept
{ *this = *this + dd; return *this; }

inline constexpr weekday& weekday::operator-=(const days& dd) noexcept
{ *this = *this - dd; return *this; }


class weekday_indexed {
private:
    poly::chrono::weekday wd_;
    unsigned char          index_;
public:
    weekday_indexed() = default;
    inline constexpr weekday_indexed(const poly::chrono::weekday& wd_val, unsigned index_val) noexcept
        : wd_{wd_val}, index_(index_val) {}
    inline constexpr poly::chrono::weekday weekday() const noexcept { return wd_; }
    inline constexpr unsigned                 index() const noexcept { return index_; }
    inline constexpr bool ok() const noexcept { return wd_.ok() && index_ >= 1 && index_ <= 5; }
};

inline constexpr
bool operator==(const weekday_indexed& lhs, const weekday_indexed& rhs) noexcept
{ return lhs.weekday() == rhs.weekday() && lhs.index() == rhs.index(); }

inline constexpr
bool operator!=(const weekday_indexed& lhs, const weekday_indexed& rhs) noexcept
{ return !(lhs == rhs); }


class weekday_last {
private:
    poly::chrono::weekday wd_;
public:
    explicit constexpr weekday_last(const poly::chrono::weekday& val) noexcept
        : wd_{val} {}
    constexpr poly::chrono::weekday weekday() const noexcept { return wd_; }
    constexpr bool ok() const noexcept { return wd_.ok(); }
};

inline constexpr
bool operator==(const weekday_last& lhs, const weekday_last& rhs) noexcept
{ return lhs.weekday() == rhs.weekday(); }

inline constexpr
bool operator!=(const weekday_last& lhs, const weekday_last& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
weekday_indexed weekday::operator[](unsigned index) const noexcept { return weekday_indexed{*this, index}; }

inline constexpr
weekday_last    weekday::operator[](last_spec) const noexcept { return weekday_last{*this}; }


inline constexpr last_spec last{};
inline constexpr weekday   Sunday{0};
inline constexpr weekday   Monday{1};
inline constexpr weekday   Tuesday{2};
inline constexpr weekday   Wednesday{3};
inline constexpr weekday   Thursday{4};
inline constexpr weekday   Friday{5};
inline constexpr weekday   Saturday{6};

inline constexpr month January{1};
inline constexpr month February{2};
inline constexpr month March{3};
inline constexpr month April{4};
inline constexpr month May{5};
inline constexpr month June{6};
inline constexpr month July{7};
inline constexpr month August{8};
inline constexpr month September{9};
inline constexpr month October{10};
inline constexpr month November{11};
inline constexpr month December{12};


class month_day {
private:
   chrono::month m_;
   chrono::day   d;
public:
    month_day() = default;
    constexpr month_day(const chrono::month& m_val, const chrono::day& dval) noexcept
        : m_{m_val}, d{dval} {}
    inline constexpr chrono::month month() const noexcept { return m_; }
    inline constexpr chrono::day   day()   const noexcept { return d; }
    constexpr bool ok() const noexcept;
};

inline constexpr
bool month_day::ok() const noexcept
{
    if (!m_.ok()) return false;
    const unsigned dval = static_cast<unsigned>(d);
    if (dval < 1 || dval > 31) return false;
    if (dval <= 29) return true;
//  Now we've got either 30 or 31
    const unsigned m_val = static_cast<unsigned>(m_);
    if (m_val == 2) return false;
    if (m_val == 4 || m_val == 6 || m_val == 9 || m_val == 11)
        return dval == 30;
    return true;
}

inline constexpr
bool operator==(const month_day& lhs, const month_day& rhs) noexcept
{ return lhs.month() == rhs.month() && lhs.day() == rhs.day(); }

inline constexpr
bool operator!=(const month_day& lhs, const month_day& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
month_day operator/(const month& lhs, const day& rhs) noexcept
{ return month_day{lhs, rhs}; }

constexpr
month_day operator/(const day& lhs, const month& rhs) noexcept
{ return rhs / lhs; }

inline constexpr
month_day operator/(const month& lhs, int rhs) noexcept
{ return lhs / day(rhs); }

constexpr
month_day operator/(int lhs, const day& rhs) noexcept
{ return month(lhs) / rhs; }

constexpr
month_day operator/(const day& lhs, int rhs) noexcept
{ return month(rhs) / lhs; }


inline constexpr
bool operator< (const month_day& lhs, const month_day& rhs) noexcept
{ return lhs.month() != rhs.month() ? lhs.month() < rhs.month() : lhs.day() < rhs.day(); }

inline constexpr
bool operator> (const month_day& lhs, const month_day& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const month_day& lhs, const month_day& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const month_day& lhs, const month_day& rhs) noexcept
{ return !(lhs < rhs); }



class month_day_last {
private:
    chrono::month m_;
public:
    explicit constexpr month_day_last(const chrono::month& val) noexcept
        : m_{val} {}
    inline constexpr chrono::month month() const noexcept { return m_; }
    inline constexpr bool ok() const noexcept { return m_.ok(); }
};

inline constexpr
bool operator==(const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return lhs.month() == rhs.month(); }

inline constexpr
bool operator!=(const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return lhs.month() < rhs.month(); }

inline constexpr
bool operator> (const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const month_day_last& lhs, const month_day_last& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr
month_day_last operator/(const month& lhs, last_spec) noexcept
{ return month_day_last{lhs}; }

inline constexpr
month_day_last operator/(last_spec, const month& rhs) noexcept
{ return month_day_last{rhs}; }

inline constexpr
month_day_last operator/(int lhs, last_spec) noexcept
{ return month_day_last{month(lhs)}; }

inline constexpr
month_day_last operator/(last_spec, int rhs) noexcept
{ return month_day_last{month(rhs)}; }


class month_weekday {
private:
    chrono::month m_;
    chrono::weekday_indexed wd_i;
public:
    month_weekday() = default;
    constexpr month_weekday(const chrono::month& m_val, const chrono::weekday_indexed& wd_ival) noexcept
        : m_{m_val}, wd_i{wd_ival} {}
    inline constexpr chrono::month                     month() const noexcept { return m_; }
    inline constexpr chrono::weekday_indexed weekday_indexed() const noexcept { return wd_i; }
    inline constexpr bool                                 ok() const noexcept { return m_.ok() && wd_i.ok(); }
};

inline constexpr
bool operator==(const month_weekday& lhs, const month_weekday& rhs) noexcept
{ return lhs.month() == rhs.month() && lhs.weekday_indexed() == rhs.weekday_indexed(); }

inline constexpr
bool operator!=(const month_weekday& lhs, const month_weekday& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
month_weekday operator/(const month& lhs, const weekday_indexed& rhs) noexcept
{ return month_weekday{lhs, rhs}; }

inline constexpr
month_weekday operator/(int lhs, const weekday_indexed& rhs) noexcept
{ return month_weekday{month(lhs), rhs}; }

inline constexpr
month_weekday operator/(const weekday_indexed& lhs, const month& rhs) noexcept
{ return month_weekday{rhs, lhs}; }

inline constexpr
month_weekday operator/(const weekday_indexed& lhs, int rhs) noexcept
{ return month_weekday{month(rhs), lhs}; }


class month_weekday_last {
    chrono::month        m_;
    chrono::weekday_last wd_l;
  public:
    constexpr month_weekday_last(const chrono::month& m_val, const chrono::weekday_last& wd_lval) noexcept
        : m_{m_val}, wd_l{wd_lval} {}
    inline constexpr chrono::month               month() const noexcept { return m_; }
    inline constexpr chrono::weekday_last weekday_last() const noexcept { return wd_l; }
    inline constexpr bool                           ok() const noexcept { return m_.ok() && wd_l.ok(); }
};

inline constexpr
bool operator==(const month_weekday_last& lhs, const month_weekday_last& rhs) noexcept
{ return lhs.month() == rhs.month() && lhs.weekday_last() == rhs.weekday_last(); }

inline constexpr
bool operator!=(const month_weekday_last& lhs, const month_weekday_last& rhs) noexcept
{ return !(lhs == rhs); }


inline constexpr
month_weekday_last operator/(const month& lhs, const weekday_last& rhs) noexcept
{ return month_weekday_last{lhs, rhs}; }

inline constexpr
month_weekday_last operator/(int lhs, const weekday_last& rhs) noexcept
{ return month_weekday_last{month(lhs), rhs}; }

inline constexpr
month_weekday_last operator/(const weekday_last& lhs, const month& rhs) noexcept
{ return month_weekday_last{rhs, lhs}; }

inline constexpr
month_weekday_last operator/(const weekday_last& lhs, int rhs) noexcept
{ return month_weekday_last{month(rhs), lhs}; }


class year_month {
    chrono::year  yr_;
    chrono::month m_;
public:
    year_month() = default;
    constexpr year_month(const chrono::year& yr_val, const chrono::month& m_val) noexcept
        : yr_{yr_val}, m_{m_val} {}
    inline constexpr chrono::year  year()  const noexcept { return yr_; }
    inline constexpr chrono::month month() const noexcept { return m_; }
    inline constexpr year_month& operator+=(const months& dm) noexcept { this->m_ += dm; return *this; }
    inline constexpr year_month& operator-=(const months& dm) noexcept { this->m_ -= dm; return *this; }
    inline constexpr year_month& operator+=(const years& dy)  noexcept { this->yr_ += dy; return *this; }
    inline constexpr year_month& operator-=(const years& dy)  noexcept { this->yr_ -= dy; return *this; }
    inline constexpr bool ok() const noexcept { return yr_.ok() && m_.ok(); }
};

inline constexpr
year_month operator/(const year& yr_, const month& m_) noexcept { return year_month{yr_, m_}; }

inline constexpr
year_month operator/(const year& yr_, int m_) noexcept { return year_month{yr_, month(m_)}; }

inline constexpr
bool operator==(const year_month& lhs, const year_month& rhs) noexcept
{ return lhs.year() == rhs.year() && lhs.month() == rhs.month(); }

inline constexpr
bool operator!=(const year_month& lhs, const year_month& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const year_month& lhs, const year_month& rhs) noexcept
{ return lhs.year() != rhs.year() ? lhs.year() < rhs.year() : lhs.month() < rhs.month(); }

inline constexpr
bool operator> (const year_month& lhs, const year_month& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const year_month& lhs, const year_month& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const year_month& lhs, const year_month& rhs) noexcept
{ return !(lhs < rhs); }

constexpr year_month operator+(const year_month& lhs, const months& rhs) noexcept
{
    int dmi = static_cast<int>(static_cast<unsigned>(lhs.month())) - 1 + rhs.count();
    const int dy = (dmi >= 0 ? dmi : dmi-11) / 12;
    dmi = dmi - dy * 12 + 1;
    return (lhs.year() + years(dy)) / month(static_cast<unsigned>(dmi));
}

constexpr year_month operator+(const months& lhs, const year_month& rhs) noexcept
{ return rhs + lhs; }

constexpr year_month operator+(const year_month& lhs, const years& rhs) noexcept
{ return (lhs.year() + rhs) / lhs.month(); }

constexpr year_month operator+(const years& lhs, const year_month& rhs) noexcept
{ return rhs + lhs; }

constexpr months     operator-(const year_month& lhs, const year_month& rhs) noexcept
{ return (lhs.year() - rhs.year()) + months(static_cast<unsigned>(lhs.month()) - static_cast<unsigned>(rhs.month())); }

constexpr year_month operator-(const year_month& lhs, const months& rhs) noexcept
{ return lhs + -rhs; }

constexpr year_month operator-(const year_month& lhs, const years& rhs) noexcept
{ return lhs + -rhs; }

class year_month_day_last;

class year_month_day {
private:
    chrono::year  yr_;
    chrono::month m_;
    chrono::day   d;
public:
     year_month_day() = default;
     inline constexpr year_month_day(
            const chrono::year& yr_val, const chrono::month& m_val, const chrono::day& dval) noexcept
            : yr_{yr_val}, m_{m_val}, d{dval} {}
    constexpr year_month_day(const year_month_day_last& yr_mdl) noexcept;
    inline constexpr year_month_day(const sys_days& sysd) noexcept
            : year_month_day(detail_from_days(sysd.time_since_epoch())) {}

    constexpr year_month_day& operator+=(const months& dm) noexcept;
    constexpr year_month_day& operator-=(const months& dm) noexcept;
    constexpr year_month_day& operator+=(const years& dy)  noexcept;
    constexpr year_month_day& operator-=(const years& dy)  noexcept;

    inline constexpr chrono::year   year() const noexcept { return yr_; }
    inline constexpr chrono::month month() const noexcept { return m_; }
    inline constexpr chrono::day     day() const noexcept { return d; }

    inline constexpr operator   sys_days() const noexcept          { return   sys_days{detail_to_days()}; }

    constexpr bool             ok() const noexcept;

    static constexpr year_month_day detail_from_days(days d) noexcept;
    constexpr days detail_to_days() const noexcept;
};


// https://howardhinnant.github.io/date_algorithms.html#civil_from_days
inline constexpr
year_month_day
year_month_day::detail_from_days(days d) noexcept
{
    static_assert(poly::numeric_limits<unsigned>::digits >= 18, "");
    static_assert(poly::numeric_limits<int>::digits >= 20     , "");
    const int      z_ = d.count() + 719468;
    const int      era_ = (z_ >= 0 ? z_ : z_ - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z_ - era_ * 146097);              // [0, 146096]
    const unsigned yr_oe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const int      yr_r = static_cast<int>(yr_oe) + era_ * 400;
    const unsigned doy = doe - (365 * yr_oe + yr_oe/4 - yr_oe/100);              // [0, 365]
    const unsigned m_p = (5 * doy + 2)/153;                                       // [0, 11]
    const unsigned dy = doy - (153 * m_p + 2)/5 + 1;                            // [1, 31]
    const unsigned m_th = m_p + (m_p < 10 ? 3 : -9);                              // [1, 12]
    return year_month_day{chrono::year{yr_r + (m_th <= 2)}, chrono::month{m_th}, chrono::day{dy}};
}

// https://howardhinnant.github.io/date_algorithms.html#days_from_civil
inline constexpr days year_month_day::detail_to_days() const noexcept
{
    static_assert(poly::numeric_limits<unsigned>::digits >= 18, "");
    static_assert(poly::numeric_limits<int>::digits >= 20     , "");

    const int      yr_r  = static_cast<int>(yr_) - (m_ <= February);
    const unsigned m_th = static_cast<unsigned>(m_);
    const unsigned dy  = static_cast<unsigned>(d);

    const int      era_ = (yr_r >= 0 ? yr_r : yr_r - 399) / 400;
    const unsigned yr_oe = static_cast<unsigned>(yr_r - era_ * 400);                // [0, 399]
    const unsigned doy = (153 * (m_th + (m_th > 2 ? -3 : 9)) + 2) / 5 + dy-1;  // [0, 365]
    const unsigned doe = yr_oe * 365 + yr_oe/4 - yr_oe/100 + doy;                // [0, 146096]
    return days{era_ * 146097 + static_cast<int>(doe) - 719468};
}

inline constexpr
bool operator==(const year_month_day& lhs, const year_month_day& rhs) noexcept
{ return lhs.year() == rhs.year() && lhs.month() == rhs.month() && lhs.day() == rhs.day(); }

inline constexpr
bool operator!=(const year_month_day& lhs, const year_month_day& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const year_month_day& lhs, const year_month_day& rhs) noexcept
{
    if (lhs.year() < rhs.year()) return true;
    if (lhs.year() > rhs.year()) return false;
    if (lhs.month() < rhs.month()) return true;
    if (lhs.month() > rhs.month()) return false;
    return lhs.day() < rhs.day();
}

inline constexpr
bool operator> (const year_month_day& lhs, const year_month_day& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const year_month_day& lhs, const year_month_day& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const year_month_day& lhs, const year_month_day& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr
year_month_day operator/(const year_month& lhs, const day& rhs) noexcept
{ return year_month_day{lhs.year(), lhs.month(), rhs}; }

inline constexpr
year_month_day operator/(const year_month& lhs, int rhs) noexcept
{ return lhs / day(rhs); }

inline constexpr
year_month_day operator/(const year& lhs, const month_day& rhs) noexcept
{ return lhs / rhs.month() / rhs.day(); }

inline constexpr
year_month_day operator/(int lhs, const month_day& rhs) noexcept
{ return year(lhs) / rhs; }

inline constexpr
year_month_day operator/(const month_day& lhs, const year& rhs) noexcept
{ return rhs / lhs; }

inline constexpr
year_month_day operator/(const month_day& lhs, int rhs) noexcept
{ return year(rhs) / lhs; }


inline constexpr
year_month_day operator+(const year_month_day& lhs, const months& rhs) noexcept
{ return (lhs.year()/lhs.month() + rhs)/lhs.day(); }

inline constexpr
year_month_day operator+(const months& lhs, const year_month_day& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
year_month_day operator-(const year_month_day& lhs, const months& rhs) noexcept
{ return lhs + -rhs; }

inline constexpr
year_month_day operator+(const year_month_day& lhs, const years& rhs) noexcept
{ return (lhs.year() + rhs) / lhs.month() / lhs.day(); }

inline constexpr
year_month_day operator+(const years& lhs, const year_month_day& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
year_month_day operator-(const year_month_day& lhs, const years& rhs) noexcept
{ return lhs + -rhs; }

inline constexpr year_month_day& year_month_day::operator+=(const months& dm) noexcept { *this = *this + dm; return *this; }
inline constexpr year_month_day& year_month_day::operator-=(const months& dm) noexcept { *this = *this - dm; return *this; }
inline constexpr year_month_day& year_month_day::operator+=(const years& dy)  noexcept { *this = *this + dy; return *this; }
inline constexpr year_month_day& year_month_day::operator-=(const years& dy)  noexcept { *this = *this - dy; return *this; }

class year_month_day_last {
private:
    chrono::year           yr_;
    chrono::month_day_last m_dl;
public:
     constexpr year_month_day_last(const year& yr_val, const month_day_last& m_dlval) noexcept
        : yr_{yr_val}, m_dl{m_dlval} {}

     constexpr year_month_day_last& operator+=(const months& m_) noexcept;
     constexpr year_month_day_last& operator-=(const months& m_) noexcept;
     constexpr year_month_day_last& operator+=(const years& yr_)  noexcept;
     constexpr year_month_day_last& operator-=(const years& yr_)  noexcept;

     inline constexpr chrono::year                     year() const noexcept { return yr_; }
     inline constexpr chrono::month                   month() const noexcept { return m_dl.month(); }
     inline constexpr chrono::month_day_last month_day_last() const noexcept { return m_dl; }
            constexpr chrono::day                       day() const noexcept;
     inline constexpr operator                     sys_days() const noexcept { return   sys_days{year()/month()/day()}; }
     inline constexpr bool                               ok() const noexcept { return yr_.ok() && m_dl.ok(); }
};

inline constexpr
chrono::day year_month_day_last::day() const noexcept
{
    constexpr chrono::day d[] =
    {
        chrono::day(31), chrono::day(28), chrono::day(31),
        chrono::day(30), chrono::day(31), chrono::day(30),
        chrono::day(31), chrono::day(31), chrono::day(30),
        chrono::day(31), chrono::day(30), chrono::day(31)
    };
    return (month() != February || !yr_.is_leap()) && month().ok() ?
        d[static_cast<unsigned>(month()) - 1] : chrono::day{29};
}

inline constexpr
bool operator==(const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{ return lhs.year() == rhs.year() && lhs.month_day_last() == rhs.month_day_last(); }

inline constexpr
bool operator!=(const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{ return !(lhs == rhs); }

inline constexpr
bool operator< (const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{
    if (lhs.year() < rhs.year()) return true;
    if (lhs.year() > rhs.year()) return false;
    return lhs.month_day_last() < rhs.month_day_last();
}

inline constexpr
bool operator> (const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{ return rhs < lhs; }

inline constexpr
bool operator<=(const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{ return !(rhs < lhs);}

inline constexpr
bool operator>=(const year_month_day_last& lhs, const year_month_day_last& rhs) noexcept
{ return !(lhs < rhs); }

inline constexpr year_month_day_last operator/(const year_month& lhs, last_spec) noexcept
{ return year_month_day_last{lhs.year(), month_day_last{lhs.month()}}; }

inline constexpr year_month_day_last operator/(const year& lhs, const month_day_last& rhs) noexcept
{ return year_month_day_last{lhs, rhs}; }

inline constexpr year_month_day_last operator/(int lhs, const month_day_last& rhs) noexcept
{ return year_month_day_last{year{lhs}, rhs}; }

inline constexpr year_month_day_last operator/(const month_day_last& lhs, const year& rhs) noexcept
{ return rhs / lhs; }

inline constexpr year_month_day_last operator/(const month_day_last& lhs, int rhs) noexcept
{ return year{rhs} / lhs; }


inline constexpr
year_month_day_last operator+(const year_month_day_last& lhs, const months& rhs) noexcept
{ return (lhs.year() / lhs.month() + rhs) / last; }

inline constexpr
year_month_day_last operator+(const months& lhs, const year_month_day_last& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
year_month_day_last operator-(const year_month_day_last& lhs, const months& rhs) noexcept
{ return lhs + (-rhs); }

inline constexpr
year_month_day_last operator+(const year_month_day_last& lhs, const years& rhs) noexcept
{ return year_month_day_last{lhs.year() + rhs, lhs.month_day_last()}; }

inline constexpr
year_month_day_last operator+(const years& lhs, const year_month_day_last& rhs) noexcept
{ return rhs + lhs; }

inline constexpr
year_month_day_last operator-(const year_month_day_last& lhs, const years& rhs) noexcept
{ return lhs + (-rhs); }

inline constexpr year_month_day_last& year_month_day_last::operator+=(const months& dm) noexcept { *this = *this + dm; return *this; }
inline constexpr year_month_day_last& year_month_day_last::operator-=(const months& dm) noexcept { *this = *this - dm; return *this; }
inline constexpr year_month_day_last& year_month_day_last::operator+=(const years& dy)  noexcept { *this = *this + dy; return *this; }
inline constexpr year_month_day_last& year_month_day_last::operator-=(const years& dy)  noexcept { *this = *this - dy; return *this; }

inline constexpr year_month_day::year_month_day(const year_month_day_last& yr_mdl) noexcept
    : yr_{yr_mdl.year()}, m_{yr_mdl.month()}, d{yr_mdl.day()} {}

inline constexpr bool year_month_day::ok() const noexcept
{
    if (!yr_.ok() || !m_.ok()) return false;
    return chrono::day{1} <= d && d <= (yr_ / m_ / last).day();
}

template <class DurationT>
class hh_mm_ss
{
private:
    static_assert(detail_is_duration<DurationT>::value, "template parameter of hh_mm_ss must be a poly::chrono::duration");
    using CommonType = typename common_type<DurationT, chrono::seconds>::type;

    static constexpr uint64_t pow10_(unsigned exp_)
    {
        uint64_t ret = 1;
        for (unsigned i_ = 0; i_ < exp_; ++i_)
            ret *= 10U;
        return ret;
    }

    static constexpr unsigned width_(uint64_t n_, uint64_t d = 10, unsigned w_ = 0)
    {
        if (n_ >= 2 && d != 0 && w_ < 19)
            return 1 + width_(n_, d % n_ * 10, w_ + 1);
        return 0;
    }

public:
    static unsigned constexpr fractional_width = width_(CommonType::period::den) < 19 ?
                                                 width_(CommonType::period::den) : 6u;
    using precision = duration<typename CommonType::rep, ratio<1, pow10_(fractional_width)>>;

    constexpr hh_mm_ss() noexcept : hh_mm_ss{DurationT::zero()} {}

    constexpr explicit hh_mm_ss(DurationT d) noexcept :
            is_neg_(d < DurationT(0)),
            h_(duration_cast<chrono::hours>  (abs(d))),
            m_(duration_cast<chrono::minutes>(abs(d) - hours())),
            s(duration_cast<chrono::seconds>(abs(d) - hours() - minutes())),
            f_(duration_cast<precision>      (abs(d) - hours() - minutes() - seconds()))
        {}

    constexpr bool is_negative()        const noexcept { return is_neg_; }
    constexpr chrono::hours hours()     const noexcept { return h_; }
    constexpr chrono::minutes minutes() const noexcept { return m_; }
    constexpr chrono::seconds seconds() const noexcept { return s; }
    constexpr precision subseconds()    const noexcept { return f_; }

    constexpr precision to_duration() const noexcept
    {
        auto dur = h_ + m_ + s + f_;
        return is_neg_ ? -dur : dur;
    }

    constexpr explicit operator precision() const noexcept { return to_duration(); }

private:
    bool            is_neg_;
    chrono::hours   h_;
    chrono::minutes m_;
    chrono::seconds s;
    precision       f_;
};

constexpr bool is_am(const hours& h_) noexcept { return h_ >= hours(0) && h_ < hours(12); }
constexpr bool is_pm(const hours& h_) noexcept { return h_ >= hours(12) && h_ < hours(24); }

constexpr hours make12(const hours& h_) noexcept
{
    if      (h_ == hours(0)) return hours(12);
    else if (h_ <= hours(12)) return h_;
    else                       return h_ - hours(12);
}

constexpr hours make24(const hours& h_, bool is_pm_) noexcept
{
    if (is_pm_)
        return h_ == hours(12) ? h_ : h_ + hours(12);
    else
        return h_ == hours(12) ? hours(0) : h_;
}
} // chrono

inline namespace literals
{
  inline namespace chrono_literals
  {

    constexpr poly::chrono::hours operator""_hours(unsigned long long _h)
    {
        return poly::chrono::hours(static_cast<chrono::hours::rep>(_h));
    }

    constexpr poly::chrono::minutes operator""_min(unsigned long long m_)
    {
        return poly::chrono::minutes(static_cast<chrono::minutes::rep>(m_));
    }

    constexpr poly::chrono::seconds operator""_sec(unsigned long long s)
    {
        return poly::chrono::seconds(static_cast<chrono::seconds::rep>(s));
    }

    constexpr poly::chrono::milliseconds operator""_ms(unsigned long long m_s)
    {
        return poly::chrono::milliseconds(static_cast<chrono::milliseconds::rep>(m_s));
    }

    constexpr poly::chrono::microseconds operator""_us(unsigned long long _us)
    {
        return poly::chrono::microseconds(static_cast<chrono::microseconds::rep>(_us));
    }

    constexpr poly::chrono::day operator ""_day(unsigned long long d) noexcept
    {
        return chrono::day(static_cast<unsigned>(d));
    }

    constexpr poly::chrono::year operator ""_year(unsigned long long yr_) noexcept
    {
        return chrono::year(static_cast<int>(yr_));
    }

      constexpr poly::chrono::month operator ""_month(unsigned long long mon_) noexcept
      {
          return chrono::month(static_cast<unsigned>(mon_));
      }

    constexpr poly::chrono::years operator ""_years(unsigned long long yr_) noexcept
    {
        return poly::chrono::years(static_cast<int>(yr_));
    }

    #ifdef POLY_CHRONO_ENABLE_DOUBLE
    constexpr chrono::duration<long double, ratio<3600,1>> operator""_hour(long double _h)
    {
        return chrono::duration<long double, ratio<3600,1>>(_h);
    }
    constexpr chrono::duration<long double, micro> operator""_us(long double _us)
    {
        return chrono::duration<long double, micro> (_us);
    }
    constexpr chrono::duration<long double, milli> operator""_ms(long double m_s)
    {
        return chrono::duration<long double, milli>(m_s);
    }
    constexpr chrono::duration<long double> operator""_sec(long double s)
    {
        return chrono::duration<long double> (s);
    }
    constexpr chrono::duration<long double, ratio<60,1>> operator""_min(long double m_)
    {
        return chrono::duration<long double, ratio<60,1>> (m_);
    }
    #endif
}}

namespace chrono { // hoist the literals into namespace poly::chrono
   using namespace literals::chrono_literals;
}

} // namespace poly

#ifndef POLY_CHRONO_NO_LITERALS
using namespace poly::chrono_literals;
#endif
