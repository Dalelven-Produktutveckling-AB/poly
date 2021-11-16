///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2018 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#pragma once

#include <limits.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#include "type_traits.hpp"

#define POLY_DETAIL_LOG10_OF_2(x) (((x) * 301) / 1000)

#if defined(POLY_NO_CPP_NAN_SUPPORT) || defined(NAN)
  #if defined(NAN)
    #define POLY_NAN  (double)NAN
    #define POLY_NANF (float)NAN
    #define POLY_NANL (long double)NAN
    #define POLY_HAS_NAN true
  #else
    #define POLY_NAN  (double)0.0
    #define POLY_NANF (float)0.0
    #define POLY_NANL (long double)0.0
    #define POLY_HAS_NAN false
  #endif
#else
  #define POLY_NAN  nan("")
  #define POLY_NANF nanf("")
  #define POLY_NANL nanl("")
  #define POLY_HAS_NAN true
#endif

#if !defined(LDBL_MIN) && defined(DBL_MIN)
  // Looks like we don't have those macros defined.
  // That probably means that 'long double' is the same size as 'double'.
  #define LDBL_MIN        DBL_MIN
  #define LDBL_MAX        DBL_MAX
  #define LDBL_EPSILON    DBL_EPSILON
  #define HUGE_VALL       HUGE_VAL
  #define LDBL_MANT_DIG   DBL_MANT_DIG
  #define LDBL_DIG        DBL_DIG
  #define LDBL_MIN_EXP    DBL_MIN_EXP
  #define LDBL_MIN_10_EXP DBL_MIN_10_EXP
  #define LDBL_MAX_EXP    DBL_MAX_EXP
  #define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#endif

namespace poly
{
  enum float_round_style
  {
    round_indeterminate       = -1,
    round_toward_zero         = 0,
    round_to_nearest          = 1,
    round_toward_infinity     = 2,
    round_toward_neg_infinity = 3,
  };

  enum float_denorm_style
  {
    denorm_indeterminate = -1,
    denorm_absent        = 0,
    denorm_present       = 1
  };


  class poly_integral_limits
  {
  public:

    static const bool is_specialized    = true;
    static const bool is_integer        = true;
    static const bool is_exact          = true;
    static const int  max_digits10      = 0;
    static const int  radix             = 2;
    static const int  min_exponent      = 0;
    static const int  min_exponent10    = 0;
    static const int  max_exponent      = 0;
    static const int  max_exponent10    = 0;
    static const bool has_infinity      = false;
    static const bool has_quiet_NaN     = false;
    static const bool has_signaling_NaN = false;
    static const bool has_denorm_loss   = false;
    static const bool is_iec559         = false;
    static const bool is_bounded        = true;
    static const bool traps             = false;
    static const bool tinyness_before   = false;
    static const float_denorm_style has_denorm = denorm_absent;
    static const float_round_style round_style = round_toward_zero;
  };

  class poly_floating_point_limits
  {
  public:

    static const bool is_specialized    = true;
    static const bool is_signed         = true;
    static const bool is_integer        = false;
    static const bool is_exact          = false;
    static const int radix              = 2;
    static const bool has_infinity      = true;
    static const bool has_quiet_NaN     = POLY_HAS_NAN;
    static const bool has_signaling_NaN = POLY_HAS_NAN;
    static const bool has_denorm_loss   = false;
    static const bool is_iec559         = false;
    static const bool is_bounded        = true;
    static const bool is_modulo         = false;
    static const bool traps             = false;
    static const bool tinyness_before   = false;
    static const float_denorm_style has_denorm = denorm_indeterminate;
    static const float_round_style round_style = round_indeterminate;

    static float round_error() { return float(0.5); }
  };

  //***************************************************************************
  // Default
  template <typename T>
  class numeric_limits;

  template <typename T>
  class numeric_limits<const T> : public numeric_limits<T> { };

  template <typename T>
  class numeric_limits<volatile T> : public numeric_limits<T> { };

  template <typename T>
  class numeric_limits<const volatile T> : public numeric_limits<T> { };

  //***************************************************************************
  // bool
  template<>
  class numeric_limits<bool> : public poly_integral_limits
  {
  public:

    static const int  digits    = 1;
    static const int  digits10  = 0;
    static const bool is_signed = false;
    static const bool is_modulo = false;

    static bool min() { return false; }
    static bool max() { return true; }
    static bool lowest() { return false; }
    static bool epsilon() { return false; }
    static bool round_error() { return false; }
    static bool denorm_min() { return false; }
    static bool infinity() { return false; }
    static bool quiet_NaN() { return false; }
    static bool signaling_NaN() { return false; }
  };

  //***************************************************************************
  // char
  template<>
  class numeric_limits<char> : public poly_integral_limits
  {
  public:

    static const int digits     = (CHAR_BIT * sizeof(char)) - (poly::is_signed<char>::value ? 1 : 0);
    static const int digits10   = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = poly::is_signed<char>::value;
    static const bool is_modulo = false;

    static char min() { return char(CHAR_MIN); }
    static char max() { return char(CHAR_MAX); }
    static char lowest() { return char(CHAR_MIN); }
    static char epsilon() { return 0; }
    static char round_error() { return 0; }
    static char denorm_min() { return 0; }
    static char infinity() { return 0; }
    static char quiet_NaN() { return 0; }
    static char signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned char
  template<>
  class numeric_limits<unsigned char> : public poly_integral_limits
  {
  public:

    static const int digits     = (CHAR_BIT * sizeof(unsigned char)) - (poly::is_signed<unsigned char>::value ? 1 : 0);
    static const int digits10   = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static unsigned char min() { return 0U; }
    static unsigned char max() { return UCHAR_MAX; }
    static unsigned char lowest() { return 0U; }
    static unsigned char epsilon() { return 0U; }
    static unsigned char round_error() { return 0U; }
    static unsigned char denorm_min() { return 0U; }
    static unsigned char infinity() { return 0U; }
    static unsigned char quiet_NaN() { return 0U; }
    static unsigned char signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // signed char
  template<>
  class numeric_limits<signed char> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(char)) - (poly::is_signed<char>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = true;
    static const bool is_modulo = false;

    static signed char min() { return SCHAR_MIN; }
    static signed char max() { return SCHAR_MAX; }
    static signed char lowest() { return SCHAR_MIN; }
    static signed char epsilon() { return 0; }
    static signed char round_error() { return 0; }
    static signed char denorm_min() { return 0; }
    static signed char infinity() { return 0; }
    static signed char quiet_NaN() { return 0; }
    static signed char signaling_NaN() { return 0; }
  };

#if (ETL_NO_LARGE_CHAR_SUPPORT == false)
  //***************************************************************************
  // char16_t
  template<>
  class numeric_limits<char16_t> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(char16_t)) - (poly::is_signed<char16_t>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static char16_t min() { return 0U; }
    static char16_t max() { return UINT_LEAST16_MAX; }
    static char16_t lowest() { return 0U; }
    static char16_t epsilon() { return 0U; }
    static char16_t round_error() { return 0U; }
    static char16_t denorm_min() { return 0U; }
    static char16_t infinity() { return 0U; }
    static char16_t quiet_NaN() { return 0U; }
    static char16_t signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // char32_t
  template<>
  class numeric_limits<char32_t> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(char32_t)) - (poly::is_signed<char32_t>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static char32_t min() { return 0U; }
    static char32_t max() { return UINT_LEAST32_MAX; }
    static char32_t lowest() { return 0U; }
    static char32_t epsilon() { return 0U; }
    static char32_t round_error() { return 0U; }
    static char32_t denorm_min() { return 0U; }
    static char32_t infinity() { return 0U; }
    static char32_t quiet_NaN() { return 0U; }
    static char32_t signaling_NaN() { return 0U; }
  };

#endif

  //***************************************************************************
  // wchar_t
  template<>
  class numeric_limits<wchar_t> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(wchar_t)) - (poly::is_signed<wchar_t>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = poly::is_signed<wchar_t>::value;
    static const bool is_modulo = poly::is_unsigned<wchar_t>::value;

    static wchar_t min() { return WCHAR_MIN; }
    static wchar_t max() { return WCHAR_MAX; }
    static wchar_t lowest() { return WCHAR_MIN; }
    static wchar_t epsilon() { return wchar_t(0); }
    static wchar_t round_error() { return wchar_t(0); }
    static wchar_t denorm_min() { return wchar_t(0); }
    static wchar_t infinity() { return wchar_t(0); }
    static wchar_t quiet_NaN() { return wchar_t(0); }
    static wchar_t signaling_NaN() { return wchar_t(0); }
  };

  //***************************************************************************
  // short
  template<>
  class numeric_limits<short> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(short)) - (poly::is_signed<short>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = true;
    static const bool is_modulo = false;

    static short min() { return SHRT_MIN; }
    static short max() { return SHRT_MAX; }
    static short lowest() { return SHRT_MIN; }
    static short epsilon() { return 0; }
    static short round_error() { return 0; }
    static short denorm_min() { return 0; }
    static short infinity() { return 0; }
    static short quiet_NaN() { return 0; }
    static short signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned short
  template<>
  class numeric_limits<unsigned short> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(unsigned short)) - (poly::is_signed<unsigned short>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static unsigned short min() { return 0U; }
    static unsigned short max() { return USHRT_MAX; }
    static unsigned short lowest() { return 0U; }
    static unsigned short epsilon() { return 0U; }
    static unsigned short round_error() { return 0U; }
    static unsigned short denorm_min() { return 0U; }
    static unsigned short infinity() { return 0U; }
    static unsigned short quiet_NaN() { return 0U; }
    static unsigned short signaling_NaN() { return 0U; }

  };

  //***************************************************************************
  // int
  template<>
  class numeric_limits<int> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(int)) - (poly::is_signed<int>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = true;
    static const bool is_modulo = false;

    static int min() { return INT_MIN; }
    static int max() { return INT_MAX; }
    static int lowest() { return INT_MIN; }
    static int epsilon() { return 0; }
    static int round_error() { return 0; }
    static int denorm_min() { return 0; }
    static int infinity() { return 0; }
    static int quiet_NaN() { return 0; }
    static int signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned int
  template<>
  class numeric_limits<unsigned int> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(unsigned int)) - (poly::is_signed<unsigned int>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static unsigned int min() { return 0U; }
    static unsigned int max() { return UINT_MAX; }
    static unsigned int lowest() { return 0U; }
    static unsigned int epsilon() { return 0U; }
    static unsigned int round_error() { return 0U; }
    static unsigned int denorm_min() { return 0U; }
    static unsigned int infinity() { return 0U; }
    static unsigned int quiet_NaN() { return 0U; }
    static unsigned int signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // long
  template<>
  class numeric_limits<long> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(long)) - (poly::is_signed<long>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = true;
    static const bool is_modulo = false;

    static long min() { return LONG_MIN; }
    static long max() { return LONG_MAX; }
    static long lowest() { return LONG_MIN; }
    static long epsilon() { return 0; }
    static long round_error() { return 0; }
    static long denorm_min() { return 0; }
    static long infinity() { return 0; }
    static long quiet_NaN() { return 0; }
    static long signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned long
  template<>
  class numeric_limits<unsigned long> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(unsigned long)) - (poly::is_signed<unsigned long>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static unsigned long min() { return 0U; }
    static unsigned long max() { return ULONG_MAX; }
    static unsigned long lowest() { return 0U; }
    static unsigned long epsilon() { return 0U; }
    static unsigned long round_error() { return 0U; }
    static unsigned long denorm_min() { return 0U; }
    static unsigned long infinity() { return 0U; }
    static unsigned long quiet_NaN() { return 0U; }
    static unsigned long signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // long long
  template<>
  class numeric_limits<long long> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(long long)) - (poly::is_signed<long long>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = true;
    static const bool is_modulo = false;

    static long long min() { return LLONG_MIN; }
    static long long max() { return LLONG_MAX; }
    static long long lowest() { return LLONG_MIN; }
    static long long epsilon() { return 0; }
    static long long round_error() { return 0; }
    static long long denorm_min() { return 0; }
    static long long infinity() { return 0; }
    static long long quiet_NaN() { return 0; }
    static long long signaling_NaN() { return 0; }
  };

  //***************************************************************************
  // unsigned long long
  template<>
  class numeric_limits<unsigned long long> : public poly_integral_limits
  {
  public:

    static const int digits = (CHAR_BIT * sizeof(unsigned long long)) - (poly::is_signed<unsigned long long>::value ? 1 : 0);
    static const int digits10 = POLY_DETAIL_LOG10_OF_2(digits);
    static const bool is_signed = false;
    static const bool is_modulo = true;

    static unsigned long long min() { return 0U; }
    static unsigned long long max() { return ULLONG_MAX; }
    static unsigned long long lowest() { return 0U; }
    static unsigned long long epsilon() { return 0U; }
    static unsigned long long round_error() { return 0U; }
    static unsigned long long denorm_min() { return 0U; }
    static unsigned long long infinity() { return 0U; }
    static unsigned long long quiet_NaN() { return 0U; }
    static unsigned long long signaling_NaN() { return 0U; }
  };

  //***************************************************************************
  // float
  template<>
  class numeric_limits<float> : public poly_floating_point_limits
  {
  public:

    static float min() { return FLT_MIN; }
    static float max() { return FLT_MAX; }
    static float lowest() { return -FLT_MAX; }
    static float epsilon() { return FLT_EPSILON; }
    static float denorm_min() { return FLT_MIN; }
    static float infinity() { return HUGE_VALF; }
    static float quiet_NaN() { return POLY_NANF; }
    static float signaling_NaN() { return POLY_NANF; }

    static const int digits       = FLT_MANT_DIG;
    static const int digits10     = FLT_DIG;
    static const int max_digits10 = POLY_DETAIL_LOG10_OF_2(FLT_MANT_DIG) + 2;

    static const int min_exponent   = FLT_MIN_EXP;
    static const int min_exponent10 = FLT_MIN_10_EXP;
    static const int max_exponent   = FLT_MAX_EXP;
    static const int max_exponent10 = FLT_MAX_10_EXP;
  };

  //***************************************************************************
  // double
  template<>
  class numeric_limits<double> : public poly_floating_point_limits
  {
  public:

    static double min() { return DBL_MIN; }
    static double max() { return DBL_MAX; }
    static double lowest() { return -DBL_MAX; }
    static double epsilon() { return DBL_EPSILON; }
    static double denorm_min() { return DBL_MIN; }
    static double infinity() { return HUGE_VAL; }
    static double quiet_NaN() { return POLY_NAN; }
    static double signaling_NaN() { return POLY_NAN; }

    static const int digits       = DBL_MANT_DIG;
    static const int digits10     = DBL_DIG;
    static const int max_digits10 = POLY_DETAIL_LOG10_OF_2(DBL_MANT_DIG) + 2;

    static const int min_exponent   = DBL_MIN_EXP;
    static const int min_exponent10 = DBL_MIN_10_EXP;
    static const int max_exponent   = DBL_MAX_EXP;
    static const int max_exponent10 = DBL_MAX_10_EXP;
  };

  //***************************************************************************
  // long double
  template<>
  class numeric_limits<long double> : public poly_floating_point_limits
  {
  public:

    static long double min() { return LDBL_MIN; }
    static long double max() { return LDBL_MAX; }
    static long double lowest() { return -LDBL_MAX; }
    static long double epsilon() { return LDBL_EPSILON; }
    static long double denorm_min() { return LDBL_MIN; }
    static long double infinity() { return HUGE_VALL; }
    static long double quiet_NaN() { return POLY_NANL; }
    static long double signaling_NaN() { return POLY_NANL; }

    static const int digits       = LDBL_MANT_DIG;
    static const int digits10     = LDBL_DIG;
    static const int max_digits10 = POLY_DETAIL_LOG10_OF_2(LDBL_MANT_DIG) + 2;

    static const int min_exponent   = LDBL_MIN_EXP;
    static const int min_exponent10 = LDBL_MIN_10_EXP;
    static const int max_exponent   = LDBL_MAX_EXP;
    static const int max_exponent10 = LDBL_MAX_10_EXP;
  };
}