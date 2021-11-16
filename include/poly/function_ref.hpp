///
// function_ref - A low-overhead non-owning function
// Written in 2017 by Simon Brand (@TartanLlama)
//
// To the extent possible under law, the author(s) have dedicated all
// copyright and related and neighboring rights to this software to the
// public domain worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication
// along with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
///

#pragma once

#ifndef TL_FUNCTION_REF_HPP
#define TL_FUNCTION_REF_HPP

#define TL_FUNCTION_REF_VERSION_MAJOR 1
#define TL_FUNCTION_REF_VERSION_MINOR 0
#define TL_FUNCTION_REF_VERSION_PATCH 0

#if (defined(_MSC_VER) && _MSC_VER == 1900)
/// \exclude
#define TL_FUNCTION_REF_MSVC2015
#endif

#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
/// \exclude
#define TL_FUNCTION_REF_GCC49
#endif

#if (defined(__GNUC__) && __GNUC__ == 5 && __GNUC_MINOR__ <= 4 &&              \
     !defined(__clang__))
/// \exclude
#define TL_FUNCTION_REF_GCC54
#endif

#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
// GCC < 5 doesn't support overloading on const&& for member functions
/// \exclude
#define TL_FUNCTION_REF_NO_CONSTRR
#endif

#if __cplusplus > 201103L
/// \exclude
#define TL_FUNCTION_REF_CXX14
#endif

// constexpr implies const in C++11, not C++14
#if (__cplusplus == 201103L || defined(TL_FUNCTION_REF_MSVC2015) ||            \
     defined(TL_FUNCTION_REF_GCC49)) &&                                        \
    !defined(TL_FUNCTION_REF_GCC54)
/// \exclude
#define TL_FUNCTION_REF_11_CONSTEXPR
#else
/// \exclude
#define TL_FUNCTION_REF_11_CONSTEXPR constexpr
#endif

#include "function.hpp"
#include "utility.hpp"
#include "type_traits.hpp"

namespace poly {
    namespace detail {
        namespace fnref {
// C++14-style aliases for brevity
            template <class T> using remove_const_t = typename poly::remove_const<T>::type;
            template <class T>
            using remove_reference_t = typename poly::remove_reference<T>::type;
            template <class T> using decay_t = typename poly::decay<T>::type;
            template <bool E, class T = void>
            using enable_if_t = typename poly::enable_if<E, T>::type;
            template <bool B, class T, class F>
            using conditional_t = typename poly::conditional<B, T, F>::type;

// poly::invoke from C++17
// https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround
            template <typename Fn, typename... Args,
                    typename = enable_if_t<poly::is_member_pointer<decay_t<Fn>>::value>,
                    int = 0>
            constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
            noexcept(poly::mem_fn(f)(poly::forward<Args>(args)...)))
            -> decltype(poly::mem_fn(f)(poly::forward<Args>(args)...)) {
                return poly::mem_fn(f)(poly::forward<Args>(args)...);
            }

            template <typename Fn, typename... Args,
                    typename = enable_if_t<!poly::is_member_pointer<decay_t<Fn>>{}>>
            constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
            noexcept(poly::forward<Fn>(f)(poly::forward<Args>(args)...)))
            -> decltype(poly::forward<Fn>(f)(poly::forward<Args>(args)...)) {
                return poly::forward<Fn>(f)(poly::forward<Args>(args)...);
            }

// poly::invoke_result from C++17
            template <class F, class, class... Us> struct invoke_result_impl;

            template <class F, class... Us>
            struct invoke_result_impl<
                    F, decltype(poly::detail::fnref::invoke(poly::declval<F>(), poly::declval<Us>()...), void()),
                    Us...> {
                using type = decltype(poly::detail::fnref::invoke(poly::declval<F>(), poly::declval<Us>()...));
            };

            template <class F, class... Us>
            using invoke_result = invoke_result_impl<F, void, Us...>;

            template <class F, class... Us>
            using invoke_result_t = typename invoke_result<F, Us...>::type;

            template <class, class R, class F, class... Args>
            struct is_invocable_r_impl : poly::false_type {};

            template <class R, class F, class... Args>
            struct is_invocable_r_impl<
                    typename poly::is_convertible<invoke_result_t<F, Args...>, R>::type, R, F, Args...>
                    : poly::true_type {};

            template <class R, class F, class... Args>
            using is_invocable_r = is_invocable_r_impl<poly::true_type, R, F, Args...>;

        } // namespace detail
    } // namespace fnref

/// A lightweight non-owning reference to a callable.
///
/// Example usage:
///
/// ```cpp
/// void foo (function_ref<int(int)> func) {
///     poly::cout << "Result is " << func(21); //42
/// }
///
/// foo([](int i) { return i*2; });
    template <class F> class function_ref;

/// Specialization for function types.
    template <class R, class... Args> class function_ref<R(Args...)> {
    public:
        constexpr function_ref() noexcept = delete;

        /// Creates a `function_ref` which refers to the same callable as `rhs`.
        constexpr function_ref(const function_ref<R(Args...)> &rhs) noexcept = default;

        /// Constructs a `function_ref` referring to `f`.
        ///
        /// \synopsis template <typename F> constexpr function_ref(F &&f) noexcept
        template <typename F,
                detail::fnref::enable_if_t<
                        !poly::is_same<detail::fnref::decay_t<F>, function_ref>::value &&
                        detail::fnref::is_invocable_r<R, F &&, Args...>::value> * = nullptr>
        TL_FUNCTION_REF_11_CONSTEXPR function_ref(F &&f) noexcept
                : obj_(const_cast<void*>(reinterpret_cast<const void *>(poly::addressof(f)))) {
            callback_ = [](void *obj, Args... args) -> R {
                return detail::fnref::invoke(
                        *reinterpret_cast<typename poly::add_pointer<F>::type>(obj),
                        poly::forward<Args>(args)...);
            };
        }

        /// Makes `*this` refer to the same callable as `rhs`.
        TL_FUNCTION_REF_11_CONSTEXPR function_ref<R(Args...)> &
        operator=(const function_ref<R(Args...)> &rhs) noexcept = default;

        /// Makes `*this` refer to `f`.
        ///
        /// \synopsis template <typename F> constexpr function_ref &operator=(F &&f) noexcept;
        template <typename F,
                detail::fnref::enable_if_t<detail::fnref::is_invocable_r<R, F &&, Args...>::value>
                * = nullptr>
        TL_FUNCTION_REF_11_CONSTEXPR function_ref<R(Args...)> &operator=(F &&f) noexcept {
            obj_ = reinterpret_cast<void *>(poly::addressof(f));
            callback_ = [](void *obj, Args... args) {
                return detail::fnref::invoke(
                        *reinterpret_cast<typename poly::add_pointer<F>::type>(obj),
                        poly::forward<Args>(args)...);
            };

            return *this;
        }

        /// Swaps the referred callables of `*this` and `rhs`.
        constexpr void swap(function_ref<R(Args...)> &rhs) noexcept {
            poly::swap(obj_, rhs.obj_);
            poly::swap(callback_, rhs.callback_);
        }

        /// Call the stored callable with the given arguments.
        R operator()(Args... args) const {
            return callback_(obj_, poly::forward<Args>(args)...);
        }

    private:
        void *obj_ = nullptr;
        R (*callback_)(void *, Args...) = nullptr;
    };

/// Swaps the referred callables of `lhs` and `rhs`.
    template <typename R, typename... Args>
    constexpr void swap(function_ref<R(Args...)> &lhs,
                        function_ref<R(Args...)> &rhs) noexcept {
        lhs.swap(rhs);
    }

#if __cplusplus >= 201703L
    template <typename R, typename... Args>
function_ref(R (*)(Args...))->function_ref<R(Args...)>;

// TODO, will require some kind of callable traits
// template <typename F>
// function_ref(F) -> function_ref</* deduced if possible */>;
#endif
} // namespace tl

#endif