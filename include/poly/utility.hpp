#pragma once

///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2016 jwellbelove

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

#ifndef POLY_UTILITY_DETAIL_UTILITY_INCLUDED
#define POLY_UTILITY_DETAIL_UTILITY_INCLUDED

#include "type_traits.hpp"

///\defgroup utility utility
///\ingroup utilities

namespace poly
{
  //******************************************************************************
  template <typename T>
  constexpr typename poly::remove_reference<T>::type&& move(T&& t) noexcept
  {
    return static_cast<typename poly::remove_reference<T>::type&&>(t);
  }

  //******************************************************************************
  template <typename T>
  constexpr T&& forward(typename poly::remove_reference<T>::type& t) noexcept
  {
    return static_cast<T&&>(t);
  }

  template <typename T>
  constexpr T&& forward(typename poly::remove_reference<T>::type&& t) noexcept
  {
    return static_cast<T&&>(t);
  }

    template<class T>
    typename poly::enable_if<poly::is_object<T>::value, T*>::type  addressof(T& arg) noexcept
    {
        return reinterpret_cast<T*>(
                &const_cast<char&>(
                        reinterpret_cast<const volatile char&>(arg)));
    }

    template<class T>
    typename poly::enable_if<!poly::is_object<T>::value, T*>::type addressof(T& arg) noexcept
    {
        return &arg;
    }

  // We can't have std::swap and poly::swap templates coexisting in the unit tests
  // as the compiler will be unable to decide of which one to use, due to ADL.
  //***************************************************************************
  // swap
  template <typename T>
  void swap(T& a, T& b) noexcept
  {
    T temp(poly::move(a));
    a = poly::move(b);
    b = poly::move(temp);
  }

  template< class T, size_t N >
  void swap(T(&a)[N], T(&b)[N]) noexcept
  {
    for (size_t i = 0; i < N; ++i)
    {
      swap(a[i], b[i]);
    }
  }

  //******************************************************************************
  template <typename T1, typename T2>
  struct pair
  {
    typedef T1 first_type;
    typedef T2 second_type;

    T1 first;
    T2 second;

    /// Default constructor
    constexpr pair()
      : first(T1())
      , second(T2())
    {
    }

    /// Constructor from parameters
    constexpr pair(const T1& a, const T2& b)
      : first(a)
      , second(b)
    {
    }

    /// Move constructor from parameters
    template <typename U1, typename U2>
    constexpr pair(U1&& a, U2&& b)
      : first(poly::forward<U1>(a))
      , second(poly::forward<U2>(b))
    {
    }

    /// Copy constructor
    template <typename U1, typename U2>
    constexpr pair(const pair<U1, U2>& other)
      : first(other.first)
      , second(other.second)
    {
    }

    /// Move constructor
    template <typename U1, typename U2>
    constexpr pair(pair<U1, U2>&& other)
      : first(poly::forward<U1>(other.first))
      , second(poly::forward<U2>(other.second))
    {
    }

    void swap(pair<T1, T2>& other)
    {
      using poly::swap;

      swap(first, other.first);
      swap(second, other.second);
    }

    template <typename U1, typename U2>
    pair<U1, U2>& operator =(const pair<U1, U2>& other)
    {
      first = other.first;
      second = other.second;

      return *this;
    }

    template <typename U1, typename U2>
    pair<U1, U2>& operator =(pair<U1, U2>&& other)
    {
      first = poly::forward<U1>(other.first);
      second = poly::forward<U2>(other.second);

      return *this;
    }
  };

//*************************************************************************
/// Template deduction guides.
//*************************************************************************
  template <typename T1, typename T2>
  pair(T1, T2) ->pair<T1, T2>;

  //******************************************************************************
  template <typename T1, typename T2>
  inline pair<T1, T2> make_pair(T1&& a, T2&& b)
  {
    return pair<T1, T2>(poly::forward<T1>(a), poly::forward<T2>(b));
  }

  //******************************************************************************
  template <typename T1, typename T2>
  inline void swap(pair<T1, T2>& a, pair<T1, T2>& b)
  {
    a.swap(b);
  }

  //******************************************************************************
  template <typename T1, typename T2>
  inline bool operator ==(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return (a.first == b.first) && (a.second == b.second);
  }

  template <typename T1, typename T2>
  inline bool operator !=(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return !(a == b);
  }

  template <typename T1, typename T2>
  inline bool operator <(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return (a.first < b.first) ||
      (!(b.first < a.first) && (a.second < b.second));
  }

  template <typename T1, typename T2>
  inline bool operator >(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return (b < a);
  }

  template <typename T1, typename T2>
  inline bool operator <=(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return !(b < a);
  }

  template <typename T1, typename T2>
  inline bool operator >=(const pair<T1, T2>& a, const pair<T1, T2>& b)
  {
    return !(a < b);
  }

  //***************************************************************************
  /// exchange (const)
  //***************************************************************************
  template <typename T>
  T exchange(T& object, const T& new_value)
  {
    T old_value = object;
    object = new_value;
    return old_value;
  }

  template <typename T, typename U>
  T exchange(T& object, const U& new_value)
  {
    T old_value = object;
    object = new_value;
    return old_value;
  }

  //***************************************************************************
  /// as_const
  //***************************************************************************
  template <typename T>
  typename poly::add_const_t<T>& as_const(T& t)
  {
    return t;
  }

  //******************************************************************************
  /// 2D coordinate type.
  template <typename T>
  struct coordinate_2d
  {
    coordinate_2d() = default;

    coordinate_2d(T x_, T y_)
      : x(x_)
      , y(y_)
    {
    }

    friend bool operator ==(const coordinate_2d& lhs, const coordinate_2d& rhs)
    {
      return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }

    friend bool operator !=(const coordinate_2d& lhs, const coordinate_2d& rhs)
    {
      return !(lhs == rhs);
    }

    T x = 0;
    T y = 0;
  };
}

#endif
