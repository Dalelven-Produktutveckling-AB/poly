#pragma once

#if __has_include(<type_traits>)
#include <type_traits>
#include <cstddef>
namespace poly {
  using std::declval;
  using std::integral_constant;
  using std::declval;

  template <bool B>
  using bool_constant = integral_constant<bool, B>;
  using true_type = bool_constant<true>;
  using false_type = bool_constant<false>;
 
  // primary type categories:
  using std::is_void;
  using std::is_null_pointer;
  using std::is_integral;
  using std::is_floating_point;
  using std::is_array;
  using std::is_pointer;
  using std::is_lvalue_reference;
  using std::is_rvalue_reference;
  using std::is_member_object_pointer;
  using std::is_member_function_pointer;
  using std::is_enum;
  using std::is_union;
  using std::is_class;
  using std::is_function;
 
  // composite type categories:
  using std::is_reference;
  using std::is_arithmetic;
  using std::is_fundamental;
  using std::is_object;
  using std::is_scalar;
  using std::is_compound;
  using std::is_member_pointer;
 
  // type properties:
  using std::is_const;
  using std::is_volatile;
  using std::is_trivial;
  using std::is_trivially_copyable;
  using std::is_standard_layout;
  using std::is_pod;
  using std::is_empty;
  using std::is_polymorphic;
  using std::is_abstract;
  using std::is_final;
  using std::is_aggregate;
  using std::is_signed;
  using std::is_unsigned;
  using std::is_constructible;
  using std::is_default_constructible;
  using std::is_copy_constructible;
  using std::is_move_constructible;
  using std::is_assignable;
  using std::is_copy_assignable;
  using std::is_move_assignable;
  using std::is_swappable_with;
  using std::is_swappable;
  using std::is_destructible;
  using std::is_trivially_constructible;
  using std::is_trivially_default_constructible;
  using std::is_trivially_copy_constructible;
  using std::is_trivially_move_constructible;
  using std::is_trivially_assignable;
  using std::is_trivially_copy_assignable;
  using std::is_trivially_move_assignable;
  using std::is_trivially_destructible;
  using std::is_nothrow_constructible;
  using std::is_nothrow_default_constructible;
  using std::is_nothrow_copy_constructible;
  using std::is_nothrow_move_constructible;
  using std::is_nothrow_assignable;
  using std::is_nothrow_copy_assignable;
  using std::is_nothrow_move_assignable;
  using std::is_nothrow_swappable_with;
  using std::is_nothrow_swappable;
  using std::is_nothrow_destructible;
  using std::has_virtual_destructor;
  using std::has_unique_object_representations;
 
  // type property queries:
  using std::alignment_of;
  using std::rank;
  using std::extent;
 
  // type relations:
  using std::is_same;
  using std::is_base_of;
  using std::is_convertible;
  using std::is_invocable;
  using std::is_invocable_r;
  using std::is_nothrow_invocable;
  using std::is_nothrow_invocable_r;
 
  // const-volatile modifications:
  using std::remove_const;
  using std::remove_volatile;
  using std::remove_cv;
  using std::add_const;
  using std::add_volatile;
  using std::add_cv;
  template <class T>
  using remove_const_t = typename remove_const<T>::type;
  template <class T>
  using remove_volatile_t = typename remove_volatile<T>::type;
  template <class T>
  using remove_cv_t = typename remove_cv<T>::type;
  template <class T>
  using add_const_t = typename add_const<T>::type;
  template <class T>
  using add_volatile_t = typename add_volatile<T>::type;
  template <class T>
  using add_cv_t = typename add_cv<T>::type;
 
  // reference modifications:
  using std::remove_reference;
  using std::add_lvalue_reference;
  using std::add_rvalue_reference;
  template <class T>
  using remove_reference_t = typename remove_reference<T>::type;
  template <class T>
  using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
  template <class T>
  using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
 
  // sign modifications:
  using std::make_signed;
  using std::make_unsigned;
  template <class T>
  using make_signed_t = typename make_signed<T>::type;
  template <class T>
  using make_unsigned_t = typename make_unsigned<T>::type;
 
  // array modifications:
  using std::remove_extent;
  using std::remove_all_extents;
  template <class T>
  using remove_extent_t = typename remove_extent<T>::type;
  template <class T>
  using remove_all_extents_t = typename remove_all_extents<T>::type;
 
  // pointer modifications:
  using std::remove_pointer;
  using std::add_pointer;
  template <class T>
  using remove_pointer_t = typename remove_pointer<T>::type;
  template <class T>
  using add_pointer_t = typename add_pointer<T>::type;
 
  // other transformations:
  using std::aligned_storage;
  using std::aligned_union;
  using std::decay;
  using std::enable_if;
  using std::conditional;

    template <class...>
    using void_t = void;

    // primary template (used for zero types)
    template <class...>
    struct common_type {};

    //////// one type
    template <class T>
    struct common_type<T> : common_type<T, T> {};

    //////// two types
    template <class T1, class T2>
    using cond_t = decltype(false ? std::declval<T1>() : std::declval<T2>());

    template <class T1, class T2, class=void>
    struct common_type_2_impl {};

    template <class T1, class T2>
    struct common_type_2_impl<T1, T2, void_t<cond_t<T1, T2>>> {
        using type = typename poly::decay<cond_t<T1, T2>>::type;
    };

    template <class T1, class T2>
    struct common_type<T1, T2>
            : common_type_2_impl<typename poly::decay<T1>::type,
                    typename poly::decay<T2>::type>
    {};

    //////// 3+ types
    template <class AlwaysVoid, class T1, class T2, class...R>
    struct common_type_multi_impl {};

    template <class T1, class T2, class...R>
    struct common_type_multi_impl<
            void_t<typename common_type<T1, T2>::type>, T1, T2, R...>
            : common_type<typename common_type<T1, T2>::type, R...> {};


    template <class T1, class T2, class... R>
    struct common_type<T1, T2, R...>
            : common_type_multi_impl<void, T1, T2, R...> {};
  using std::underlying_type;
  using std::invoke_result;
  template <size_t Len,
  size_t Align> 
  using aligned_storage_t = typename aligned_storage<Len, Align>::type;
  template <size_t Len, class... Types>
  using aligned_union_t = typename aligned_union<Len, Types...>::type;
  template <class T>
  using decay_t = typename decay<T>::type;
  template <bool b, class T = void>
  using enable_if_t = typename enable_if<b, T>::type;
  template <bool b, class T, class F>
  using conditional_t = typename conditional<b, T, F>::type;
  template <class... T>
  using common_type_t = typename common_type<T...>::type;
  template <class T>
  using underlying_type_t = typename underlying_type<T>::type;
  template <class F, class... ArgTypes> 
  using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;
 
  // logical operator traits:
  template<class... B> struct conjunction;
  template<class... B> struct disjunction;
  template<class B> struct negation;
 
  // primary type categories
  template <class T> inline constexpr bool is_void_v
  = is_void<T>::value;
  template <class T> inline constexpr bool is_null_pointer_v
  = is_null_pointer<T>::value;
  template <class T> inline constexpr bool is_integral_v
  = is_integral<T>::value;
  template <class T> inline constexpr bool is_floating_point_v
  = is_floating_point<T>::value;
  template <class T> inline constexpr bool is_array_v
  = is_array<T>::value;
  template <class T> inline constexpr bool is_pointer_v
  = is_pointer<T>::value;
  template <class T> inline constexpr bool is_lvalue_reference_v
  = is_lvalue_reference<T>::value;
  template <class T> inline constexpr bool is_rvalue_reference_v
  = is_rvalue_reference<T>::value;
  template <class T> inline constexpr bool is_member_object_pointer_v
  = is_member_object_pointer<T>::value;
  template <class T> inline constexpr bool is_member_function_pointer_v
  = is_member_function_pointer<T>::value;
  template <class T> inline constexpr bool is_enum_v
  = is_enum<T>::value;
  template <class T> inline constexpr bool is_union_v
  = is_union<T>::value;
  template <class T> inline constexpr bool is_class_v
  = is_class<T>::value;
  template <class T> inline constexpr bool is_function_v
  = is_function<T>::value;
 
  // composite type categories
  template <class T> inline constexpr bool is_reference_v
  = is_reference<T>::value;
  template <class T> inline constexpr bool is_arithmetic_v
  = is_arithmetic<T>::value;
  template <class T> inline constexpr bool is_fundamental_v
  = is_fundamental<T>::value;
  template <class T> inline constexpr bool is_object_v
  = is_object<T>::value;
  template <class T> inline constexpr bool is_scalar_v
  = is_scalar<T>::value;
  template <class T> inline constexpr bool is_compound_v
  = is_compound<T>::value;
  template <class T> inline constexpr bool is_member_pointer_v
  = is_member_pointer<T>::value;
 
  // type properties
  template <class T> inline constexpr bool is_const_v
  = is_const<T>::value;
  template <class T> inline constexpr bool is_volatile_v
  = is_volatile<T>::value;
  template <class T> inline constexpr bool is_trivial_v
  = is_trivial<T>::value;
  template <class T> inline constexpr bool is_trivially_copyable_v
  = is_trivially_copyable<T>::value;
  template <class T> inline constexpr bool is_standard_layout_v
  = is_standard_layout<T>::value;
  template <class T> inline constexpr bool is_pod_v
  = is_pod<T>::value;
  template <class T> inline constexpr bool is_empty_v
  = is_empty<T>::value;
  template <class T> inline constexpr bool is_polymorphic_v
  = is_polymorphic<T>::value;
  template <class T> inline constexpr bool is_abstract_v
  = is_abstract<T>::value;
  template <class T> inline constexpr bool is_final_v
  = is_final<T>::value;
  template <class T> inline constexpr bool is_aggregate_v
  = is_aggregate<T>::value;
  template <class T> inline constexpr bool is_signed_v
  = is_signed<T>::value;
  template <class T> inline constexpr bool is_unsigned_v
  = is_unsigned<T>::value;
  template <class T, class... Args> inline constexpr bool is_constructible_v
  = is_constructible<T, Args...>::value;
  template <class T> inline constexpr bool is_default_constructible_v
  = is_default_constructible<T>::value;
  template <class T> inline constexpr bool is_copy_constructible_v
  = is_copy_constructible<T>::value;
  template <class T> inline constexpr bool is_move_constructible_v
  = is_move_constructible<T>::value;
  template <class T, class U> inline constexpr bool is_assignable_v
  = is_assignable<T, U>::value;
  template <class T> inline constexpr bool is_copy_assignable_v
  = is_copy_assignable<T>::value;
  template <class T> inline constexpr bool is_move_assignable_v
  = is_move_assignable<T>::value;
  template <class T, class U> inline constexpr bool is_swappable_with_v
  = is_swappable_with<T, U>::value;
  template <class T> inline constexpr bool is_swappable_v
  = is_swappable<T>::value;
  template <class T> inline constexpr bool is_destructible_v
  = is_destructible<T>::value;
  template <class T, class... Args> inline constexpr bool is_trivially_constructible_v
  = is_trivially_constructible<T, Args...>::value;
  template <class T> inline constexpr bool is_trivially_default_constructible_v
  = is_trivially_default_constructible<T>::value;
  template <class T> inline constexpr bool is_trivially_copy_constructible_v
  = is_trivially_copy_constructible<T>::value;
  template <class T> inline constexpr bool is_trivially_move_constructible_v
  = is_trivially_move_constructible<T>::value;
  template <class T, class U> inline constexpr bool is_trivially_assignable_v
  = is_trivially_assignable<T, U>::value;
  template <class T> inline constexpr bool is_trivially_copy_assignable_v
  = is_trivially_copy_assignable<T>::value;
  template <class T> inline constexpr bool is_trivially_move_assignable_v
  = is_trivially_move_assignable<T>::value;
  template <class T> inline constexpr bool is_trivially_destructible_v
  = is_trivially_destructible<T>::value;
  template <class T, class... Args> inline constexpr bool is_nothrow_constructible_v
  = is_nothrow_constructible<T, Args...>::value;
  template <class T> inline constexpr bool is_nothrow_default_constructible_v
  = is_nothrow_default_constructible<T>::value;
  template <class T> inline constexpr bool is_nothrow_copy_constructible_v
  = is_nothrow_copy_constructible<T>::value;
  template <class T> inline constexpr bool is_nothrow_move_constructible_v
  = is_nothrow_move_constructible<T>::value;
  template <class T, class U> inline constexpr bool is_nothrow_assignable_v
  = is_nothrow_assignable<T, U>::value;
  template <class T> inline constexpr bool is_nothrow_copy_assignable_v
  = is_nothrow_copy_assignable<T>::value;
  template <class T> inline constexpr bool is_nothrow_move_assignable_v
  = is_nothrow_move_assignable<T>::value;
  template <class T, class U> inline constexpr bool is_nothrow_swappable_with_v
  = is_nothrow_swappable_with<T, U>::value;
  template <class T> inline constexpr bool is_nothrow_swappable_v
  = is_nothrow_swappable<T>::value;
  template <class T> inline constexpr bool is_nothrow_destructible_v
  = is_nothrow_destructible<T>::value;
  template <class T> inline constexpr bool has_virtual_destructor_v
  = has_virtual_destructor<T>::value;
  template <class T> inline constexpr bool has_unique_object_representations_v
  = has_unique_object_representations<T>::value;
 
  // type property queries
  template <class T> inline constexpr size_t alignment_of_v
  = alignment_of<T>::value;
  template <class T> inline constexpr size_t rank_v
  = rank<T>::value;
  template <class T, unsigned I = 0> inline constexpr size_t extent_v
  = extent<T, I>::value;
 
  // type relations
  template <class T, class U> inline constexpr bool is_same_v
  = is_same<T, U>::value;
  template <class Base, class Derived> inline constexpr bool is_base_of_v
  = is_base_of<Base, Derived>::value;
  template <class From, class To> inline constexpr bool is_convertible_v
  = is_convertible<From, To>::value;

  template <class Fn, class... ArgTypes> 
  inline constexpr bool is_invocable_v
    = is_invocable<Fn, ArgTypes...>::value;
  template <class R, class Fn, class... ArgTypes>
  inline constexpr bool is_invocable_r_v
    = is_invocable_r<R, Fn, ArgTypes...>::value;
  template <class Fn, class... ArgTypes>
  inline constexpr bool is_nothrow_invocable_v
    = is_nothrow_invocable<Fn, ArgTypes...>::value;
  template <class R, class Fn, class... ArgTypes>
  inline constexpr bool is_nothrow_invocable_r_v
    = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;
 
  // logical operator traits:
  template<class... B> inline constexpr bool conjunction_v = conjunction<B...>::value;
  template<class... B> inline constexpr bool disjunction_v = disjunction<B...>::value;
  template<class B> inline constexpr bool negation_v = negation<B>::value;
}
#else
// Modified etl/type_traits.h file

///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2014 jwellbelove

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

#if 0
#error THIS HEADER IS A GENERATOR. DO NOT INCLUDE.
#endif

//***************************************************************************
// THIS FILE HAS BEEN AUTO GENERATED. DO NOT EDIT THIS FILE.
//***************************************************************************

//***************************************************************************
// To generate to header file, run this at the command line.
// Note: You will need Python and COG installed.
//
// python -m cogapp -d -e -otypes.h -DHandlers=<n> types_generator.h
// Where <n> is the number of types to support.
//
// e.g.
// To generate handlers for up to 16 types...
// python -m cogapp -d -e -otype_traits.h -DIsOneOf=16 type_traits_generator.h
//
// See generate.bat
//***************************************************************************

#ifndef POLY_TYPE_TRAITS_DETAIL_TYPE_TRAITS_INCLUDED
#define POLY_TYPE_TRAITS_DETAIL_TYPE_TRAITS_INCLUDED

#include <stddef.h>
#include <stdint.h>

///\defgroup type_traits type_traits
/// A set of type traits definitions.
/// Derived from either the standard or alternate definitions, dependant on whether or not POLY_TYPE_TRAITS_DETAIL_NO_STL is defined.
/// \ingroup utilities

#define POLY_TYPE_TRAITS_DETAIL_NOT_USING_STL 1
#define POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED 1
#define POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED 1
#define POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED 1

namespace poly
{
#if POLY_TYPE_TRAITS_DETAIL_NOT_USING_STL || POLY_TYPE_TRAITS_DETAIL_CPP11_NOT_SUPPORTED

  //***************************************************************************
  /// add_rvalue_reference
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename T> struct add_rvalue_reference { using type = T && ; };
  template <typename T> struct add_rvalue_reference<T&> { using type = T & ; };
  template <>           struct add_rvalue_reference<void> { using type = void; };
  template <>           struct add_rvalue_reference<const void> { using type = const void; };
  template <>           struct add_rvalue_reference<volatile void> { using type = volatile void; };
  template <>           struct add_rvalue_reference<const volatile void> { using type = const volatile void; };
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename T>
  typename poly::add_rvalue_reference<T>::type declval() noexcept;
#endif

    //***************************************************************************
  /// remove_all_extents
  template <typename T> struct remove_all_extents { typedef T type; };
  template <typename T> struct remove_all_extents<T[]> { typedef typename remove_all_extents<T>::type type; };
  template <typename T, size_t MAXN> struct remove_all_extents<T[MAXN]> { typedef typename remove_all_extents<T>::type type; };


  //*****************************************************************************
  // Traits are defined by the ETL
  //*****************************************************************************
  //***************************************************************************
  /// integral_constant
  template <typename T, const T VALUE>
  struct integral_constant
  {
    static const T value = VALUE;

    typedef T value_type;
    typedef integral_constant<T, VALUE> type;

    operator value_type() const
    {
      return value;
    }
  };

  /// integral_constant specialisations
  typedef integral_constant<bool, false> false_type;
  typedef integral_constant<bool, true>  true_type;

  template <typename T, const T VALUE>
  const T integral_constant<T, VALUE>::value;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <bool B>
  using bool_constant = integral_constant<bool, B>;
#else
  template <bool B>
  struct bool_constant : poly::integral_constant<bool, B> { };
#endif

  //***************************************************************************
  /// negation
  template <typename T>
  struct negation : poly::bool_constant<!bool(T::value)>
  {
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool negation_v = negation<T>::value;
#endif

  //***************************************************************************
  /// remove_reference
  template <typename T> struct remove_reference { typedef T type; };
  template <typename T> struct remove_reference<T&> { typedef T type; };
  template <typename T> struct remove_reference<T&&> { typedef T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_reference_t = typename remove_reference<T>::type;
#endif

  //***************************************************************************
  /// remove_pointer
  template <typename T> struct remove_pointer { typedef T type; };
  template <typename T> struct remove_pointer<T*> { typedef T type; };
  template <typename T> struct remove_pointer<const T*> { typedef const T type; };
  template <typename T> struct remove_pointer<volatile T*> { typedef volatile T type; };
  template <typename T> struct remove_pointer<const volatile T*> { typedef const volatile T type; };
  template <typename T> struct remove_pointer<T* const> { typedef T type; };
  template <typename T> struct remove_pointer<const T* const> { typedef const T type; };
  template <typename T> struct remove_pointer<volatile T* const> { typedef volatile T type; };
  template <typename T> struct remove_pointer<const volatile T* const> { typedef const volatile T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_pointer_t = typename remove_pointer<T>::type;
#endif

  //***************************************************************************
  /// add_pointer
  template <typename T> struct add_pointer { typedef typename remove_reference<T>::type* type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_pointer_t = typename add_pointer<T>::type;
#endif

  //***************************************************************************
  /// is_const
  template <typename T> struct is_const : false_type {};
  template <typename T> struct is_const<const T> : true_type {};
  template <typename T> struct is_const<const volatile T> : true_type {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_const_v = is_const<T>::value;
#endif

  //***************************************************************************
  /// remove_const
  template <typename T> struct remove_const { typedef T type; };
  template <typename T> struct remove_const<const T> { typedef T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_const_t = typename remove_const<T>::type;
#endif

  //***************************************************************************
  /// add_const
  template <typename T> struct add_const { typedef const T type; };
  template <typename T> struct add_const<const T> { typedef const T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_const_t = typename add_const<T>::type;
#endif

  //***************************************************************************
  /// is_volatile
  template <typename T> struct is_volatile : false_type {};
  template <typename T> struct is_volatile<volatile T> : true_type {};
  template <typename T> struct is_volatile<const volatile T> : true_type {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_volatile_v = is_volatile<T>::value;
#endif

  //***************************************************************************
  /// remove_volatile
  template <typename T> struct remove_volatile { typedef T type; };
  template <typename T> struct remove_volatile<volatile T> { typedef T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_volatile_t = typename remove_volatile<T>::type;
#endif

  //***************************************************************************
  /// add_volatile
  template <typename T> struct add_volatile { typedef volatile T type; };
  template <typename T> struct add_volatile<volatile T> { typedef volatile T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_volatile_t = typename add_volatile<T>::type;
#endif

  //***************************************************************************
  /// remove_cv
  template <typename T> struct remove_cv
  {
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_cv_t = typename remove_cv<T>::type;
#endif

  template< class T >
  struct is_member_pointer_helper         : poly::false_type {};

  template< class T, class U >
  struct is_member_pointer_helper<T U::*> : poly::true_type {};

  template< class T >
  struct is_member_pointer :
      is_member_pointer_helper<typename poly::remove_cv<T>::type> {};

  //***************************************************************************
  /// add_cv
  template <typename T> struct add_cv
  {
    typedef typename add_volatile<typename add_const<T>::type>::type type;
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_cv_t = typename add_cv<T>::type;
#endif

  //***************************************************************************
  /// is_integral
  template <typename T> struct is_integral : false_type {};
  template <> struct is_integral<bool> : true_type {};
  template <> struct is_integral<char> : true_type {};
  template <> struct is_integral<unsigned char> : true_type {};
  template <> struct is_integral<signed char> : true_type {};
  template <> struct is_integral<wchar_t> : true_type {};
  template <> struct is_integral<short> : true_type {};
  template <> struct is_integral<unsigned short> : true_type {};
  template <> struct is_integral<int> : true_type {};
  template <> struct is_integral<unsigned int> : true_type {};
  template <> struct is_integral<long> : true_type {};
  template <> struct is_integral<unsigned long> : true_type {};
  template <> struct is_integral<long long> : true_type {};
  template <> struct is_integral<unsigned long long> : true_type {};
  template <typename T> struct is_integral<const T> : is_integral<T> {};
  template <typename T> struct is_integral<volatile T> : is_integral<T> {};
  template <typename T> struct is_integral<const volatile T> : is_integral<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_integral_v = is_integral<T>::value;
#endif

  //***************************************************************************
  /// is_signed
  template <typename T> struct is_signed : false_type {};
  template <> struct is_signed<char> : integral_constant<bool, (char(255) < 0)> {};
  template <> struct is_signed<wchar_t> : public poly::integral_constant<bool, static_cast<bool>(wchar_t(-1) < wchar_t(0))> {};
  template <> struct is_signed<signed char> : true_type {};
  template <> struct is_signed<short> : true_type {};
  template <> struct is_signed<int> : true_type {};
  template <> struct is_signed<long> : true_type {};
  template <> struct is_signed<long long> : true_type {};
  template <> struct is_signed<float> : true_type {};
  template <> struct is_signed<double> : true_type {};
  template <> struct is_signed<long double> : true_type {};
  template <typename T> struct is_signed<const T> : is_signed<T> {};
  template <typename T> struct is_signed<volatile T> : is_signed<T> {};
  template <typename T> struct is_signed<const volatile T> : is_signed<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_signed_v = is_signed<T>::value;
#endif

  //***************************************************************************
  /// is_unsigned
  template <typename T> struct is_unsigned : false_type {};
  template <> struct is_unsigned<bool> : true_type {};
  template <> struct is_unsigned<char> : integral_constant<bool, (char(255) > 0)> {};
  template <> struct is_unsigned<unsigned char> : true_type {};
  template <> struct is_unsigned<wchar_t> : public poly::integral_constant<bool, (wchar_t(-1) > wchar_t(0))> {};
  template <> struct is_unsigned<unsigned short> : true_type {};
  template <> struct is_unsigned<unsigned int> : true_type {};
  template <> struct is_unsigned<unsigned long> : true_type {};
  template <> struct is_unsigned<unsigned long long> : true_type {};
  template <typename T> struct is_unsigned<const T> : is_unsigned<T> {};
  template <typename T> struct is_unsigned<volatile T> : is_unsigned<T> {};
  template <typename T> struct is_unsigned<const volatile T> : is_unsigned<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
#endif

  //***************************************************************************
  /// is_floating_point
  template <typename T> struct is_floating_point : false_type {};
  template <> struct is_floating_point<float> : true_type {};
  template <> struct is_floating_point<double> : true_type {};
  template <> struct is_floating_point<long double> : true_type {};
  template <typename T> struct is_floating_point<const T> : is_floating_point<T> {};
  template <typename T> struct is_floating_point<volatile T> : is_floating_point<T> {};
  template <typename T> struct is_floating_point<const volatile T> : is_floating_point<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
#endif

  //***************************************************************************
  /// is_same
  template <typename T1, typename T2> struct is_same : public false_type {};
  template <typename T> struct is_same<T, T> : public true_type {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T1, typename T2>
  inline constexpr bool is_same_v = is_same<T1, T2>::value;
#endif

  //***************************************************************************
  /// is_void
  template<typename T> struct is_void : false_type {};
  template<> struct is_void<void> : true_type {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_void_v = is_void<T>::value;
#endif

  //***************************************************************************
  /// is_arithmetic
  template<typename T> struct is_arithmetic : integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
#endif

  //***************************************************************************
  /// is_fundamental
  template <typename T> struct is_fundamental : integral_constant<bool, is_arithmetic<T>::value || is_void<T>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
#endif

  //***************************************************************************
  /// is_compound
  template <typename T> struct is_compound : integral_constant<bool, !is_fundamental<T>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_compound_v = is_compound<T>::value;
#endif

  //***************************************************************************
  /// is_array
  template <typename T> struct is_array : false_type {};
  template <typename T> struct is_array<T[]> : true_type {};
  template <typename T, size_t MAXN> struct is_array<T[MAXN]> : true_type {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_array_v = is_array<T>::value;
#endif

  //***************************************************************************
  /// is_pointer
  template<typename T> struct is_pointer_helper : false_type {};
  template<typename T> struct is_pointer_helper<T*> : true_type {};
  template<typename T> struct is_pointer : is_pointer_helper<typename remove_cv<T>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_pointer_v = is_pointer<T>::value;
#endif

  //***************************************************************************
  /// is_reference
  template<typename T> struct is_reference_helper : false_type {};
  template<typename T> struct is_reference_helper<T&> : true_type {};
  template<typename T> struct is_reference : is_reference_helper<typename remove_cv<T>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_reference_v = is_reference<T>::value;
#endif

  //***************************************************************************
  /// is_lvalue_reference
  template<typename T> struct is_lvalue_reference_helper : false_type {};
  template<typename T> struct is_lvalue_reference_helper<T&> : true_type {};
  template<typename T> struct is_lvalue_reference : is_lvalue_reference_helper<typename remove_cv<T>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_lvalue_reference_v = poly::is_lvalue_reference<T>::value;
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  //***************************************************************************
  /// is_rvalue_reference
  template<typename T> struct is_rvalue_reference_helper : false_type {};
  template<typename T> struct is_rvalue_reference_helper<T&&> : true_type {};
  template<typename T> struct is_rvalue_reference : is_rvalue_reference_helper<typename remove_cv<T>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_rvalue_reference_v = poly::is_rvalue_reference<T>::value;
#endif
#endif

  //***************************************************************************
  /// is_pod
  /// Only fundamental and pointers types are recognised.
  template <typename T> struct is_pod : poly::integral_constant<bool, poly::is_fundamental<T>::value || poly::is_pointer<T>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_pod_v = poly::is_pod<T>::value;
#endif

  template<class T, class...Args>
  struct is_constructible: poly::bool_constant<__is_constructible(T, Args...)> {};

  template<class T, class...Args>
  inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

namespace detail
{
  template<bool, typename T, typename...Args>
  struct is_nothrow_constructible_impl: public poly::false_type {};

  template<typename T, typename...Args>
  struct is_nothrow_constructible_impl<true, T, Args...>: public poly::bool_constant<noexcept(T(poly::declval<Args>()...))> {};

  template<typename T>
  struct is_nothrow_constructible_impl<true, T>: public poly::bool_constant<noexcept(T())> {};

  template<typename T, class U>
  struct is_nothrow_constructible_impl<true, T, U>: public poly::bool_constant<noexcept(static_cast<T>(declval<U>()))> {};

  template<typename T, size_t Num>
    struct is_nothrow_constructible_impl<true, T[Num]>
    : public bool_constant<noexcept(typename remove_all_extents<T>::type())>
    { };
}

  template<class T, class...Args>
  struct is_nothrow_constructible: detail::is_nothrow_constructible_impl<__is_constructible(T, Args...), T, Args...> {};

  template<class T, class...Args>
  inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

  template <typename T, class...Args> struct is_trivially_constructible : poly::bool_constant<__is_trivially_constructible(T, Args...)> {};

  template<class T>
  struct is_default_constructible: is_constructible<T> {};

  template<class T>
  struct is_nothrow_default_constructible: is_nothrow_constructible<T> {};

  template<class T>
  struct is_copy_constructible: is_constructible<T, const T&> {};

  template<class T>
  inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

  template<class T>
  struct is_nothrow_copy_constructible: is_nothrow_constructible<T, const T&> {};

  template<class T>
  struct is_trivially_copy_constructible: is_trivially_constructible<T, const T&> {};

  template<class T>
  struct is_move_constructible: is_constructible<T, T&&> {};

  template<class T>
  struct is_nothrow_move_constructible: is_nothrow_constructible<T, T&&> {};

  template<class T>
  struct is_trivially_move_constructible: is_trivially_constructible<T, T&&> {};

  

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_constructible_v = poly::is_trivially_constructible<T>::value;
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copy_constructible_v = poly::is_trivially_copy_constructible<T>::value;
#endif

  //***************************************************************************
  /// is_trivially_destructible
  /// Only POD types are recognised.
  template <typename T> struct is_trivially_destructible : poly::is_pod<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_destructible_v = poly::is_trivially_destructible<T>::value;
#endif

  template<class T, class U>
  struct is_assignable: bool_constant<__is_assignable(T, U)> {};

namespace detail
{
  template<bool, class T, class U>
  struct is_nothrow_assignable: false_type {};

  template<class T, class U>
  struct is_nothrow_assignable<true, T, U>: bool_constant<noexcept(declval<T>() = declval<U>())> {};
}

  template<class T, class U>
  struct is_nothrow_assignable: detail::is_nothrow_assignable<__is_assignable(T, U), T, U> {};

  template<class T, class U>
  struct is_trivially_assignable: bool_constant<__is_trivially_assignable(T, U)> {};

  template<class T>
  struct is_copy_assignable: is_assignable<T, const T&> {};

  template<class T>
  struct is_nothrow_copy_assignable: is_nothrow_assignable<T, const T&> {};

  template<class T>
  struct is_trivially_copy_assignable: is_trivially_assignable<T, const T&> {};

  template<class T>
  struct is_move_assignable: is_assignable<T, T&&> {};

  template<class T>
  struct is_nothrow_move_assignable: is_nothrow_assignable<T, T&&> {};

  template<class T>
  struct is_trivially_move_assignable: is_trivially_assignable<T, T&&> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copy_assignable_v = poly::is_trivially_copy_assignable<T>::value;
#endif

  //***************************************************************************
  /// is_trivially_copyable
  /// Only POD types are recognised.
  template <typename T> struct is_trivially_copyable : poly::is_pod<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copyable_v = poly::is_trivially_copyable<T>::value;
#endif

  //***************************************************************************
  /// conditional
  template <bool B, typename T, typename F>  struct conditional { typedef T type; };
  template <typename T, typename F> struct conditional<false, T, F> { typedef F type; };

  //***************************************************************************
  /// make_signed
  template <typename T> struct make_signed { typedef  T type; };
  template <> struct make_signed<char> { typedef  signed char type; };
  template <> struct make_signed<unsigned char> { typedef  signed char type; };

  template <> struct make_signed<wchar_t>
  {
    typedef poly::conditional<sizeof(wchar_t) == sizeof(int16_t),
      int16_t,
      poly::conditional<sizeof(wchar_t) == sizeof(int32_t),
      int32_t,
      void>::type>::type type;
  };

  template <> struct make_signed<unsigned short> { typedef  short type; };
  template <> struct make_signed<unsigned int> { typedef int type; };
  template <> struct make_signed<unsigned long> { typedef  long type; };
  template <> struct make_signed<unsigned long long> { typedef long long type; };
  template <typename T> struct make_signed<const T> : add_const<typename make_signed<T>::type> {};
  template <typename T> struct make_signed<volatile T> : add_volatile<typename make_signed<T>::type> {};
  template <typename T> struct make_signed<const volatile T> : add_const<typename add_volatile<typename make_signed<T>::type>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using make_signed_t = typename make_signed<T>::type;
#endif

  //***************************************************************************
  /// make_unsigned
  template <typename T> struct make_unsigned { typedef  T type; };
  template <> struct make_unsigned<char> { typedef unsigned char type; };
  template <> struct make_unsigned<signed char> { typedef unsigned char type; };
  template <> struct make_unsigned<short> { typedef unsigned short type; };

  template <> struct make_unsigned<wchar_t>
  {
    typedef poly::conditional<sizeof(wchar_t) == sizeof(uint16_t),
      uint16_t,
      poly::conditional<sizeof(wchar_t) == sizeof(uint32_t),
      uint32_t,
      void>::type>::type type;
  };

  template <> struct make_unsigned<int> { typedef unsigned int type; };
  template <> struct make_unsigned<long> { typedef unsigned long type; };
  template <> struct make_unsigned<long long> { typedef unsigned long long type; };
  template <typename T> struct make_unsigned<const T> : add_const<typename make_unsigned<T>::type> {};
  template <typename T> struct make_unsigned<volatile T> : add_volatile<typename make_unsigned<T>::type> {};
  template <typename T> struct make_unsigned<const volatile T> : add_const<typename add_volatile<typename make_unsigned<T>::type>::type> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using make_unsigned_t = typename make_unsigned<T>::type;
#endif

  //***************************************************************************
  /// enable_if
  template <bool B, typename T = void> struct enable_if {};
  template <typename T> struct enable_if<true, T> { typedef T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <bool B, typename T = void>
  using enable_if_t = typename enable_if<B, T>::type;
#endif

  //***************************************************************************
  /// extent
  template <typename T, size_t MAXN = 0U>
  struct extent : integral_constant<size_t, 0U> {};

  template <typename T>
  struct extent<T[], 0> : integral_constant<size_t, 0U> {};

  template <typename T, size_t MAXN>
  struct extent<T[], MAXN> : integral_constant<size_t, extent<T, MAXN - 1>::value> {};

  template <typename T, size_t MAXN>
  struct extent<T[MAXN], 0> : integral_constant<size_t, MAXN> {};

  template <typename T, size_t I, size_t MAXN>
  struct extent<T[I], MAXN> : integral_constant<size_t, extent<T, MAXN - 1>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T, size_t N = 0U>
  inline constexpr size_t extent_v = extent<T, N>::value;
#endif

  //***************************************************************************
  /// remove_extent
  template <typename T> struct remove_extent { typedef T type; };
  template <typename T> struct remove_extent<T[]> { typedef T type; };
  template <typename T, size_t MAXN> struct remove_extent<T[MAXN]> { typedef T type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_extent_t = typename remove_extent<T>::type;
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_all_extents_t = typename remove_all_extents<T>::type;
#endif

  //***************************************************************************
  /// rank
  template <typename T>struct rank : integral_constant<size_t, 0> {};
  template <typename T> struct rank<T[]> : public integral_constant<size_t, rank<T>::value + 1> {};
  template <typename T, size_t MAXN> struct rank<T[MAXN]> : public integral_constant<size_t, rank<T>::value + 1> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr size_t rank_v = rank<T>::value;
#endif

  //***************************************************************************
  /// decay
  template <typename T>
  struct decay
  {
    typedef typename poly::remove_reference<T>::type U;
    typedef typename poly::conditional<poly::is_array<U>::value,
      typename poly::remove_extent<U>::type*,
      typename poly::remove_cv<U>::type>::type type;
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using decay_t = typename decay<T>::type;
#endif

  //***************************************************************************
  /// is_base_of
  template<typename TBase,
           typename TDerived>
  struct is_base_of: bool_constant<__is_base_of(TBase, TDerived)>
  {
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T1, typename T2>
  inline constexpr bool is_base_of_v = is_base_of<T1, T2>::value;
#endif

  //***************************************************************************
  /// is_class
  namespace private_type_traits
  {
    template <typename T> char test(int T::*); // Match for classes.

    struct dummy { char c[2]; };
    template <typename T> dummy test(...);     // Match for non-classes.
  }

  template <typename T>
  struct is_class : poly::integral_constant<bool, sizeof(private_type_traits::test<T>(0)) == 1U> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_class_v = is_class<T>::value;
#endif

  //***************************************************************************
  /// add_lvalue_reference
  template <typename T> struct add_lvalue_reference { typedef T& type; };
  template <typename T> struct add_lvalue_reference<T&> { typedef T& type; };
  template <>           struct add_lvalue_reference<void> { typedef void type; };
  template <>           struct add_lvalue_reference<const void> { typedef const void type; };
  template <>           struct add_lvalue_reference<volatile void> { typedef volatile void type; };
  template <>           struct add_lvalue_reference<const volatile void> { typedef const volatile void type; };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_lvalue_reference_t = typename poly::add_lvalue_reference<T>::type;
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_rvalue_reference_t = typename poly::add_rvalue_reference<T>::type;
#endif

  //***************************************************************************
  /// is_convertible
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  namespace private_type_traits
  {
    template <typename>
    using true_type_for = poly::true_type;

    template <typename T>
    auto returnable(int)->true_type_for<T()>;

    template <typename>
    auto returnable(...)->poly::false_type;

    template <typename TFrom, typename TTo>
    auto nonvoid_convertible(int)->true_type_for<decltype(poly::declval<void(&)(TTo)>()(poly::declval<TFrom>()))
    >;
    template <typename, typename>
    auto nonvoid_convertible(...)->poly::false_type;
  }

  template <typename TFrom, typename TTo>
  struct is_convertible : poly::integral_constant<bool, (decltype(private_type_traits::returnable<TTo>(0))::value &&
                                                        decltype(private_type_traits::nonvoid_convertible<TFrom, TTo>(0))::value) ||
                                                        (poly::is_void<TFrom>::value && poly::is_void<TTo>::value)> {};
  template<class T>
  struct is_nothrow_destructible: poly::bool_constant<noexcept(declval<T>().~T())> {};
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename TFrom, typename TTo >
  inline constexpr bool is_convertible_v = poly::is_convertible<TFrom, TTo>::value;
#endif

  //***************************************************************************
  /// Alignment templates.
  /// These require compiler specific intrinsics.
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED && !defined(POLY_TYPE_TRAITS_DETAIL_COMPILER_ARM5)
  template <typename T> struct alignment_of : integral_constant<size_t, alignof(T)> { };
#elif POLY_TYPE_TRAITS_DETAIL_COMPILER_MICROSOFT
  template <typename T> struct alignment_of : integral_constant<size_t, size_t(__alignof(T))> {};
#elif defined(POLY_TYPE_TRAITS_DETAIL_COMPILER_IAR) || defined(POLY_TYPE_TRAITS_DETAIL_COMPILER_TI)
  template <typename T> struct alignment_of : integral_constant<size_t, size_t(__ALIGNOF__(T))> {};
#else
  template <typename T> struct alignment_of : integral_constant<size_t, size_t(__alignof__(T))> {};
#endif

  /// Specialisation of 'alignment_of' for 'void'.
  ///\ingroup type_traits
  template <> struct alignment_of<void> : integral_constant <size_t, 0> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr size_t alignment_of_v = poly::alignment_of<T>::value;
#endif

#else // Condition = POLY_TYPE_TRAITS_DETAIL_USING_STL && POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED

  //*****************************************************************************
  // Traits are derived from the STL
  //*****************************************************************************

  //***************************************************************************
  /// integral_constant
  ///\ingroup type_traits
  template <typename T, const T VALUE>
  struct integral_constant : std::integral_constant<T, VALUE> {};

  /// integral_constant specialisations
  ///\ingroup type_traits
  typedef integral_constant<bool, false> false_type;
  typedef integral_constant<bool, true>  true_type;

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <bool B>
  using bool_constant = std::bool_constant<B>;
#else
  template <bool B>
  struct bool_constant : std::integral_constant<bool, B> { };
#endif

  //***************************************************************************
  /// negation
  ///\ingroup type_traits
#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  struct negation : std::negation<T>
  {
  };

  template <typename T>
  inline constexpr bool negation_v = std::negation_v<T>;
#endif

  //***************************************************************************
  /// remove_reference
  ///\ingroup type_traits
  template <typename T> struct remove_reference : std::remove_reference<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_reference_t = std::remove_reference_t<T>;
#endif

  //***************************************************************************
  /// remove_pointer
  ///\ingroup type_traits
  template <typename T> struct remove_pointer : std::remove_pointer<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_pointer_t = std::remove_pointer_t<T>;
#endif

  //***************************************************************************
  /// add_pointer
  ///\ingroup type_traits
  template <typename T> struct add_pointer : std::add_pointer<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_pointer_t = std::add_pointer_t<T>;
#endif

  //***************************************************************************
  /// is_const
  ///\ingroup type_traits
  template <typename T> struct is_const : std::is_const<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_const_v = std::is_const_v<T>;
#endif

  //***************************************************************************
  /// remove_const
  ///\ingroup type_traits
  template <typename T> struct remove_const : std::remove_const<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_const_t = std::remove_const_t<T>;
#endif

  //***************************************************************************
  /// add_const
  ///\ingroup type_traits
  template <typename T> struct add_const : std::add_const<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_const_t = std::add_const_t<T>;
#endif

  //***************************************************************************
  /// is_volatile
  ///\ingroup type_traits
  template <typename T> struct is_volatile : std::is_volatile<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_volatile_v = std::is_volatile_v<T>;
#endif

  //***************************************************************************
  /// remove_volatile
  ///\ingroup type_traits
  template <typename T> struct remove_volatile : std::remove_volatile<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_volatile_t = std::remove_volatile_t<T>;
#endif

  //***************************************************************************
  /// add_volatile
  ///\ingroup type_traits
  template <typename T> struct add_volatile : std::add_volatile<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_volatile_t = std::add_volatile_t<T>;
#endif

  //***************************************************************************
  /// remove_cv
  ///\ingroup type_traits
  template <typename T> struct remove_cv : std::remove_cv<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_cv_t = std::remove_cv_t<T>;
#endif

  //***************************************************************************
  /// add_cv
  ///\ingroup type_traits
  template <typename T> struct add_cv : std::add_cv<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_cv_t = std::add_cv_t<T>;
#endif

  //***************************************************************************
  /// is_integral
  ///\ingroup type_traits
  template <typename T> struct is_integral : std::is_integral<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_integral_v = std::is_integral_v<T>;
#endif

  //***************************************************************************
  /// is_signed
  ///\ingroup type_traits
  template <typename T> struct is_signed : std::is_signed<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_signed_v = std::is_signed_v<T>;
#endif

  //***************************************************************************
  /// is_unsigned
  ///\ingroup type_traits
  template <typename T> struct is_unsigned : std::is_unsigned<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_unsigned_v = std::is_unsigned_v<T>;
#endif

  //***************************************************************************
  /// is_floating_point
  ///\ingroup type_traits
  template <typename T> struct is_floating_point : std::is_floating_point<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_floating_point_v = std::is_floating_point_v<T>;
#endif

  //***************************************************************************
  /// is_same
  ///\ingroup type_traits
  template <typename T1, typename T2> struct is_same : std::is_same<T1, T2> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T1, typename T2>
  inline constexpr bool is_same_v = std::is_same_v<T1, T2>;
#endif

  //***************************************************************************
  /// is_void
  ///\ingroup type_traits
  template<typename T> struct is_void : std::is_void<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_void_v = std::is_void_v<T>;
#endif

  //***************************************************************************
  /// is_arithmetic
  ///\ingroup type_traits
  template<typename T> struct is_arithmetic : std::is_arithmetic<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_arithmetic_v = std::is_arithmetic_v<T>;
#endif

  //***************************************************************************
  /// is_fundamental
  ///\ingroup type_traits
  template <typename T> struct is_fundamental : std::is_fundamental<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_fundamental_v = std::is_fundamental_v<T>;
#endif

  //***************************************************************************
  /// is_compound
  ///\ingroup type_traits
  template <typename T> struct is_compound : std::is_compound<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_compound_v = std::is_compound_v<T>;
#endif

  //***************************************************************************
  /// is_array
  ///\ingroup type_traits
  template <typename T> struct is_array : std::is_array<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_array_v = std::is_array_v<T>;
#endif

  //***************************************************************************
  /// is_pointer
  ///\ingroup type_traits
  template<typename T> struct is_pointer : std::is_pointer<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_pointer_v = std::is_pointer_v<T>;
#endif

  //***************************************************************************
  /// is_reference
  ///\ingroup type_traits
  template<typename T> struct is_reference : std::is_reference<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_reference_v = std::is_reference_v<T>;
#endif

  //***************************************************************************
  /// is_lvalue_reference
  ///\ingroup type_traits
  template<typename T> struct is_lvalue_reference : std::is_lvalue_reference<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_lvalue_reference_v = std::is_lvalue_reference_v<T>;
#endif

  //***************************************************************************
  /// is_rvalue_reference
  ///\ingroup type_traits
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template<typename T> struct is_rvalue_reference : std::is_rvalue_reference<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_rvalue_reference_v = std::is_rvalue_reference_v<T>;
#endif
#endif

  //***************************************************************************
  /// is_pod
  ///\ingroup type_traits
  template <typename T>
  struct is_pod : std::integral_constant<bool, std::is_standard_layout<T>::value && std::is_trivial<T>::value> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_pod_v = std::is_standard_layout_v<T> && std::is_trivial_v<T>;
#endif

#if !defined(ARDUINO) && POLY_TYPE_TRAITS_DETAIL_NOT_USING_STLPORT
  //***************************************************************************
  /// is_trivially_constructible
  ///\ingroup type_traits
  template <typename T> struct is_trivially_constructible : std::is_trivially_constructible<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_constructible_v = std::is_trivially_constructible_v<T>;
#endif

  //***************************************************************************
  /// is_trivially_copy_constructible
  ///\ingroup type_traits
  template <typename T> struct is_trivially_copy_constructible : std::is_trivially_copy_constructible<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copy_constructible_v = std::is_trivially_copy_constructible_v<T>;
#endif

  //***************************************************************************
  /// is_trivially_destructible
  ///\ingroup type_traits
  template <typename T> struct is_trivially_destructible : std::is_trivially_destructible<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_destructible_v = std::is_trivially_destructible_v<T>;
#endif

  //***************************************************************************
  /// is_trivially_copy_assignable
  ///\ingroup type_traits
  template <typename T> struct is_trivially_copy_assignable : std::is_trivially_copy_assignable<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable_v<T>;
#endif

  //***************************************************************************
  /// is_trivially_copyable
  ///\ingroup type_traits
  template <typename T> struct is_trivially_copyable : std::is_trivially_copyable<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_trivially_copyable_v = std::is_trivially_copyable_v<T>;
#endif
#else
    //***************************************************************************
    /// is_trivially_constructible
    ///\ingroup type_traits
    template <typename T> struct is_trivially_constructible : std::is_pod<T> {};

  #if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
    template <typename T>
    inline constexpr bool is_trivially_constructible_v = std::is_pod_v<T>;
  #endif

    //***************************************************************************
    /// is_trivially_copy_constructible
    ///\ingroup type_traits
    template <typename T> struct is_trivially_copy_constructible : std::is_pod<T> {};

  #if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
    template <typename T>
    inline constexpr bool is_trivially_copy_constructible_v = std::is_pod_v<T>;
  #endif

    //***************************************************************************
    /// is_trivially_destructible
    ///\ingroup type_traits
    template <typename T> struct is_trivially_destructible : std::is_pod<T> {};

  #if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
    template <typename T>
    inline constexpr bool is_trivially_destructible_v = std::is_pod_v<T>;
  #endif

    //***************************************************************************
    /// is_trivially_copy_assignable
    ///\ingroup type_traits
    template <typename T> struct is_trivially_copy_assignable : std::is_pod<T> {};

  #if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
    template <typename T>
    inline constexpr bool is_trivially_copy_assignable_v = std::is_pod_v<T>;
  #endif

    //***************************************************************************
    /// is_trivially_copyable
    ///\ingroup type_traits
    template <typename T> struct is_trivially_copyable : std::is_pod<T> {};

  #if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
    template <typename T>
    inline constexpr bool is_trivially_copyable_v = std::is_pod_v<T>;
  #endif
#endif

  //***************************************************************************
  /// conditional
  ///\ingroup type_traits
  template <bool B, typename T, typename F>  struct conditional { typedef T type; };
  template <typename T, typename F> struct conditional<false, T, F> { typedef F type; };

  //***************************************************************************
  /// make_signed
  ///\ingroup type_traits
  template <typename T> struct make_signed : std::make_signed<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using make_signed_t = std::make_signed_t<T>;
#endif

  //***************************************************************************
  /// make_unsigned
  ///\ingroup type_traits
  template <typename T> struct make_unsigned : std::make_unsigned<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using make_unsigned_t = std::make_unsigned_t<T>;
#endif

  //***************************************************************************
  /// enable_if
  ///\ingroup type_traits
  template <bool B, typename T = void> struct enable_if : std::enable_if<B, T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <bool B, typename T = void>
  using enable_if_t = std::enable_if_t<B, T>;
#endif

  //***************************************************************************
  /// extent
  ///\ingroup type_traits
  template <typename T, size_t MAXN = 0U>
  struct extent : std::extent<T, MAXN> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T, size_t MAXN = 0U>
  inline constexpr size_t extent_v = std::extent_v<T, MAXN>;
#endif

  //***************************************************************************
  /// remove_extent
  ///\ingroup type_traits
  template <typename T> struct remove_extent : std::remove_extent<T> { };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_extent_t = std::remove_extent_t<T>;
#endif

  //***************************************************************************
  /// remove_all_extents
  ///\ingroup type_traits
  template <typename T> struct remove_all_extents : std::remove_all_extents<T> { };

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using remove_all_extents_t = std::remove_all_extents_t<T>;
#endif

  //***************************************************************************
  /// rank
  ///\ingroup type_traits
  template <typename T>struct rank : std::rank<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr size_t rank_v = std::rank_v<T>;
#endif

  //***************************************************************************
  /// decay
  ///\ingroup type_traits
  template <typename T> struct decay : std::decay<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using decay_t = std::decay_t<T>;
#endif

  //***************************************************************************
  /// is_base_of
  ///\ingroup type_traits
  template<typename TBase, typename TDerived> struct is_base_of : std::is_base_of<TBase, TDerived> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename TBase, typename TDerived>
  inline constexpr bool is_base_of_v = std::is_base_of_v<TBase, TDerived>;
#endif

  //***************************************************************************
  /// is_class
  template <typename T> struct is_class : std::is_class<T>{};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr bool is_class_v = is_class<T>::value;
#endif

  //***************************************************************************
  /// add_lvalue_reference
  template <typename T> struct add_lvalue_reference : std::add_lvalue_reference<T> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_lvalue_reference_t = std::add_lvalue_reference_t<T>;
#endif

  //***************************************************************************
  /// add_rvalue_reference
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename T> struct add_rvalue_reference : std::add_rvalue_reference<T> {};
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP14_SUPPORTED
  template <typename T>
  using add_rvalue_reference_t = std::add_rvalue_reference_t<T>;
#endif

  //***************************************************************************
  /// decval
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename T>
  typename std::add_rvalue_reference<T>::type declval() POLY_TYPE_TRAITS_DETAIL_NOEXCEPT;
#endif

  //***************************************************************************
  /// is_convertible
  ///\ingroup type_traits
#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename TFrom, typename TTo>
  struct is_convertible : std::is_convertible<TFrom, TTo> {};
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename TFrom, typename TTo>
  inline constexpr bool is_convertible_v = std::is_convertible_v<TFrom, TTo>;
#endif

  //***************************************************************************
  /// Alignment templates.
  ///\ingroup type_traits
  template <typename T> struct alignment_of : std::alignment_of<T> {};
  template <> struct alignment_of<void> : std::integral_constant<size_t, 0> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr size_t alignment_of_v = std::alignment_of_v<T>;
#endif

#endif // Condition = POLY_TYPE_TRAITS_DETAIL_USING_STL && POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED

  //***************************************************************************
  // ETL extended type traits.
  //***************************************************************************

  //***************************************************************************
  /// conditional_integral_constant
  // /\ingroup type_traits
  template <bool B, typename T, T TRUE_VALUE, T FALSE_VALUE>
  struct conditional_integral_constant;

  template <typename T, T TRUE_VALUE, T FALSE_VALUE>
  struct conditional_integral_constant<true, T, TRUE_VALUE, FALSE_VALUE>
  {
    static_assert(poly::is_integral<T>::value, "Not an integral type");
    static const T value = TRUE_VALUE;
  };

  template <typename T, T TRUE_VALUE, T FALSE_VALUE>
  struct conditional_integral_constant<false, T, TRUE_VALUE, FALSE_VALUE>
  {
    static_assert(poly::is_integral<T>::value, "Not an integral type");
    static const T value = FALSE_VALUE;
  };


#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  //***************************************************************************
  /// Template to determine if a type is one of a specified list.
  ///\ingroup types
  template <typename T, typename T1, typename... TRest>
  struct is_one_of
  {
    static const bool value = poly::is_same<T, T1>::value ||
                              poly::is_one_of<T, TRest...>::value;
  };

  template <typename T, typename T1>
  struct is_one_of<T, T1>
  {
    static const bool value = poly::is_same<T, T1>::value;
  };
#else
  //***************************************************************************
  /// Template to determine if a type is one of a specified list.
  ///\ingroup types
  template <typename T,
            typename T1, typename T2 = void, typename T3 = void, typename T4 = void, 
            typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void, 
            typename T9 = void, typename T10 = void, typename T11 = void, typename T12 = void, 
            typename T13 = void, typename T14 = void, typename T15 = void, typename T16 = void>
  struct is_one_of
  {
    static const bool value = 
        poly::is_same<T, T1>::value ||
        poly::is_same<T, T2>::value ||
        poly::is_same<T, T3>::value ||
        poly::is_same<T, T4>::value ||
        poly::is_same<T, T5>::value ||
        poly::is_same<T, T6>::value ||
        poly::is_same<T, T7>::value ||
        poly::is_same<T, T8>::value ||
        poly::is_same<T, T9>::value ||
        poly::is_same<T, T10>::value ||
        poly::is_same<T, T11>::value ||
        poly::is_same<T, T12>::value ||
        poly::is_same<T, T13>::value ||
        poly::is_same<T, T14>::value ||
        poly::is_same<T, T15>::value ||
        poly::is_same<T, T16>::value;
  };
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T, typename... TRest>
  inline constexpr bool is_one_of_v = poly::is_one_of<T, TRest...>::value;
#endif

  //***************************************************************************
  /// A set of templates to allow related types to be derived.
  ///\ingroup types

  // Default.
  template <typename T>
  struct types
  {
  private:

    typedef typename poly::remove_reference<typename poly::remove_cv<T>::type>::type type_t;

  public:

    typedef type_t              type;
    typedef type_t&             reference;
    typedef const type_t&       const_reference;
    typedef type_t*             pointer;
    typedef const type_t*       const_pointer;
    typedef const type_t* const const_pointer_const;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
    typedef type_t&&            rvalue_reference;
#endif
  };

  // Pointers.
  template <typename T>
  struct types<T*>
  {
  private:

    typedef typename poly::remove_reference<typename poly::remove_cv<T>::type>::type type_t;

  public:

    typedef type_t              type;
    typedef type_t&             reference;
    typedef const type_t&       const_reference;
    typedef type_t*             pointer;
    typedef const type_t*       const_pointer;
    typedef const type_t* const const_pointer_const;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
    typedef type_t&&            rvalue_reference;
#endif
  };

  // Pointers.
  template <typename T>
  struct types<T* const>
  {
  private:

    typedef typename poly::remove_reference<typename poly::remove_cv<T>::type>::type type_t;

  public:

    typedef type_t              type;
    typedef type_t&             reference;
    typedef const type_t&       const_reference;
    typedef type_t*             pointer;
    typedef const type_t* const_pointer;
    typedef const type_t* const const_pointer_const;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
    typedef type_t&&            rvalue_reference;
#endif
  };

  // References.
  template <typename T>
  struct types<T&>
  {
  private:

    typedef typename poly::remove_reference<typename poly::remove_cv<T>::type>::type type_t;

  public:

    typedef type_t              type;
    typedef type_t&             reference;
    typedef const type_t&       const_reference;
    typedef type_t*             pointer;
    typedef const type_t*       const_pointer;
    typedef const type_t* const const_pointer_const;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
    typedef type_t&&            rvalue_reference;
#endif
  };

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  // rvalue References.
  template <typename T>
  struct types<T&&>
  {
  private:

    typedef typename poly::remove_reference<typename poly::remove_cv<T>::type>::type type_t;

  public:

    typedef type_t              type;
    typedef type_t&             reference;
    typedef const type_t&       const_reference;
    typedef type_t*             pointer;
    typedef const type_t*       const_pointer;
    typedef const type_t* const const_pointer_const;

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
    typedef type_t&&            rvalue_reference;
#endif
  };
#endif

#if POLY_TYPE_TRAITS_DETAIL_CPP11_SUPPORTED
  template <typename T>
  using types_t = typename types<T>::type;

  template <typename T>
  using types_r = typename types<T>::reference;

  template <typename T>
  using types_cr = typename types<T>::const_reference;

  template <typename T>
  using types_rr = typename types<T>::rvalue_reference;

  template <typename T>
  using types_p = typename types<T>::pointer;

  template <typename T>
  using types_cp = typename types<T>::const_pointer;

  template <typename T>
  using types_cpc = typename types<T>::const_pointer_const;
#endif

  //***************************************************************************
  /// size_of
  ///\ingroup types
  template <typename T> struct size_of : poly::integral_constant<size_t, sizeof(T)> {};
  template <> struct size_of<void> : poly::integral_constant<size_t, 1U> {};

#if POLY_TYPE_TRAITS_DETAIL_CPP17_SUPPORTED
  template <typename T>
  inline constexpr size_t size_of_v = poly::size_of<T>::value;
#endif

  
template <typename...>
using void_t = void; 
 
// primary template (used for zero types)
template <class...>
struct common_type {};
 
//////// one type
template <class T>
struct common_type<T> : common_type<T, T> {};
 
//////// two types
template <class T1, class T2>
using cond_t = decltype(false ? poly::declval<T1>() : poly::declval<T2>());
 
template <class T1, class T2, class=void>
struct common_type_2_impl {};
 
template <class T1, class T2>
struct common_type_2_impl<T1, T2, void_t<cond_t<T1, T2>>> {
    using type = typename poly::decay<cond_t<T1, T2>>::type;
};
 
template <class T1, class T2>
struct common_type<T1, T2> 
  : common_type_2_impl<typename poly::decay<T1>::type, 
                       typename poly::decay<T2>::type>
{};
 
//////// 3+ types
template <class AlwaysVoid, class T1, class T2, class...R>
struct common_type_multi_impl {};
 
template <class T1, class T2, class...R>
struct common_type_multi_impl<
      void_t<typename common_type<T1, T2>::type>, T1, T2, R...>
  : common_type<typename common_type<T1, T2>::type, R...> {};
 
 
template <class T1, class T2, class... R>
struct common_type<T1, T2, R...>
  : common_type_multi_impl<void, T1, T2, R...> {};

template<class>
struct is_function : poly::false_type { };

// specialization for regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : poly::true_type {};

// specialization for function types that have cv-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) const> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile> : poly::true_type {};

// specialization for function types that have ref-qualifiers
template<class Ret, class... Args>
struct is_function<Ret(Args...) &> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) &&> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const &&> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile &&> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile &&> : poly::true_type {};

// specializations for noexcept versions of all the above (C++17 and later)

template<class Ret, class... Args>
struct is_function<Ret(Args...) noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) & noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const & noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile & noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile & noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) && noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const && noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) volatile && noexcept> : poly::true_type {};
template<class Ret, class... Args>
struct is_function<Ret(Args...) const volatile && noexcept> : poly::true_type {};

template<class T>
inline constexpr bool is_function_v = is_function<T>::value;

template< class T >
struct is_member_function_pointer_helper : poly::false_type {};

template< class T, class U>
struct is_member_function_pointer_helper<T U::*> : poly::is_function<T> {};

template< class T >
struct is_member_function_pointer
  : is_member_function_pointer_helper< typename poly::remove_cv<T>::type > {};

template<class T>
inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;


template<class T>
struct is_member_object_pointer : poly::bool_constant<
                                      poly::is_member_pointer<T>::value &&
                                      !poly::is_member_function_pointer<T>::value
                                  > {};
template<class T>
inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

namespace tt_invoke_detail {
template <class T>
struct is_reference_wrapper : poly::false_type {};

template<class T>
struct invoke_impl {
    template<class F, class... Args>
    static auto call(F&& f, Args&&... args)
        -> decltype(((F&&)f)((Args&&)args...));
};

template<class B, class MT>
struct invoke_impl<MT B::*> {
    template<class T, class Td = typename poly::decay<T>::type,
        class = typename poly::enable_if<poly::is_base_of<B, Td>::value>::type
    >
    static auto get(T&& t) -> T&&;

    template<class T, class Td = typename poly::decay<T>::type,
        class = typename poly::enable_if<is_reference_wrapper<Td>::value>::type
    >
    static auto get(T&& t) -> decltype(t.get());

    template<class T, class Td = typename poly::decay<T>::type,
        class = typename poly::enable_if<!poly::is_base_of<B, Td>::value>::type,
        class = typename poly::enable_if<!is_reference_wrapper<Td>::value>::type
    >
    static auto get(T&& t) -> decltype(*((T&&)t));

    template<class T, class... Args, class MT1,
        class = typename poly::enable_if<poly::is_function<MT1>::value>::type
    >
    static auto call(MT1 B::*pmf, T&& t, Args&&... args)
        -> decltype((invoke_impl::get(((T&&)t)).*pmf)(((Args&&)args)...));

    template<class T>
    static auto call(MT B::*pmd, T&& t)
        -> decltype(invoke_impl::get(((T&&)t)).*pmd);
};

template<class F, class... Args, class Fd = typename poly::decay<F>::type>
auto INVOKE(F&& f, Args&&... args)
    -> decltype(invoke_impl<Fd>::call(((F&&)f), ((Args&&)args)...));

} // namespace tt_invoke_detail

// Conforming C++14 implementation (is also a valid C++11 implementation):
namespace tt_invoke_detail {
template <typename AlwaysVoid, typename, typename...>
struct invoke_result { };
template <typename F, typename...Args>
struct invoke_result<decltype(void(tt_invoke_detail::INVOKE(declval<F>(), declval<Args>()...))),
                 F, Args...> {
    using type = decltype(tt_invoke_detail::INVOKE(declval<F>(), declval<Args>()...));
};
} // namespace tt_invoke_detail

template <class> struct result_of;
template <class F, class... ArgTypes>
struct result_of<F(ArgTypes...)> : tt_invoke_detail::invoke_result<void, F, ArgTypes...> {};

template <class F, class... ArgTypes>
struct invoke_result : tt_invoke_detail::invoke_result<void, F, ArgTypes...> {};

template<class F, class...ArgTypes>
using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;


template< class T >
struct is_null_pointer : poly::is_same<decltype(nullptr), poly::remove_cv_t<T>> {};

template <class Ty>
struct is_enum : bool_constant<__is_enum(Ty)> {}; // determine whether _Ty is an enumerated type

template< class T >
struct is_scalar : poly::integral_constant<bool,
                     poly::is_arithmetic<T>::value     ||
                     poly::is_enum<T>::value           ||
                     poly::is_pointer<T>::value        ||
                     poly::is_member_pointer<T>::value ||
                     poly::is_null_pointer<T>::value> {};

template <class Ty>
struct is_union : bool_constant<__is_union(Ty)> {}; // determine whether _Ty is a union

template <class Ty>
inline constexpr bool is_union_v = __is_union(Ty);

template< class T>
struct is_object : bool_constant<
                     poly::is_scalar<T>::value ||
                     poly::is_array<T>::value  ||
                     poly::is_union<T>::value  ||
                     poly::is_class<T>::value> {};
}

#endif // POLY_TYPE_TRAITS_DETAIL_TYPE_TRAITS_INCLUDED

#endif // !__has_include (<type_traits>)

namespace poly
{
struct nonesuch {
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

namespace detail {
    template <class Default, class AlwaysVoid,
            template<class...> class Op, class... Args>
    struct detector {
        using value_t = poly::false_type;
        using type = Default;
    };

    template <class Default, template<class...> class Op, class... Args>
    struct detector<Default, poly::void_t<Op<Args...>>, Op, Args...> {
        using value_t = poly::true_type;
        using type = Op<Args...>;
    };

} // namespace detail

template <template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
inline constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;
}

#if !__has_include(<type_traits>)
namespace poly
{
namespace tt_detail
{
// The primary template is used for invalid INVOKE expressions.
template<typename Result, typename Ret,
        bool = poly::is_void<Ret>::value, typename = void>
struct is_invocable_impl : poly::false_type { };

// Used for valid INVOKE and INVOKE<void> expressions.
template<typename Result, typename Ret>
struct is_invocable_impl<Result, Ret,
        /* is_void<_Ret> = */ true,
        poly::void_t<typename Result::type>>
: poly::true_type
{ };

// Used for INVOKE<R> expressions to check the implicit conversion to R.
template<typename Result, typename Ret>
struct is_invocable_impl<Result, Ret,
        /* is_void<_Ret> = */ false,
        poly::void_t<typename Result::type>>
{
private:
    // The type of the INVOKE expression.
    // Unlike declval, this doesn't add_rvalue_reference.
    static typename Result::type S_get();

    template<typename Tp>
    static void S_conv(Tp);

    // This overload is viable if INVOKE(f, args...) can convert to Tp.
    template<typename Tp, typename = decltype(S_conv<Tp>(S_get()))>
    static true_type
    S_test(int);

    template<typename Tp>
    static false_type
    S_test(...);

public:
    using type = decltype(S_test<Ret>(1));
};

template<typename Fn, typename... ArgTypes>
struct is_invocable
        : is_invocable_impl<poly::invoke_result<Fn, ArgTypes...>, void>::type
{ };
}

template<class T, class...Args>
using is_invocable = tt_detail::is_invocable<T, Args...>;

template<class T, class...Args>
inline constexpr bool is_invocable_v = is_invocable<T, Args...>::value;
}
#endif
