
//          Copyright Andreas Wass 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Implements a subset of Rusts std::Result in C++.
 */

#pragma once

#include <new>
#include <cstdint>
#include <poly/panic.hpp>
#include <poly/type_traits.hpp>
#include <poly/utility.hpp>

//          Copyright Andreas Wass 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#if defined(__cpp_lib_launder)
#define LAUNDER(x) std::launder(x)
#else
#define LAUNDER(x) x
#endif

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(nodiscard) >= 201603
#define poly_USE_NODISCARD
#endif
#endif

#ifdef poly_USE_NODISCARD
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

#ifdef _MSVC_LANG
#define poly_RESULT_CPP_VER _MSVC_LANG
#else
#define poly_RESULT_CPP_VER __cplusplus
#endif

#if !defined(poly_RESULT_CPP_VER) || poly_RESULT_CPP_VER < 201402L
#error "Must build with C++ 14 or higher, use c++17 or higher for best result"
#endif

#if poly_RESULT_CPP_VER >= 201703L
#define poly_RESULT_USE_OPTIONAL

namespace poly
{
namespace result_helper
{
template <class Fn, class... Args>
constexpr decltype(auto) invoke(Fn&& fn, Args&&... ts) {
    return poly::forward<Fn>(fn)(poly::forward<Args>(ts)...);
}

namespace detail
{
template<class T>
struct invoke_impl {
    template<class F, class... Args>
    static auto call(F&& f, Args&&... args)
    -> decltype(poly::forward<F>(f)(poly::forward<Args>(args)...));
};

template<class F, class... Args, class Fd = typename poly::decay<F>::type>
auto INVOKE(F&& f, Args&&... args)
-> decltype(invoke_impl<Fd>::call(poly::forward<F>(f), poly::forward<Args>(args)...));

template <typename AlwaysVoid, typename, typename...>
struct invoke_result { };
template <typename F, typename...Args>
struct invoke_result<decltype(void(INVOKE(poly::declval<F>(), poly::declval<Args>()...))),
        F, Args...>
{
    using type = decltype(INVOKE(poly::declval<F>(), poly::declval<Args>()...));
};
}

template <class Fn, class... Ts>
using invoke_result_t = typename detail::invoke_result<void, Fn, Ts...>::type;
} // namespace result_helper
} // namespace poly

#else
#define poly_RESULT_USE_EXPERIMENTAL_OPTIONAL

//          Copyright Andreas Wass 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

/**
 * @file Contains a std::invoke replacement implemented in C++14
 */

namespace poly
{

namespace detail
{
template <class T>
struct is_reference_wrapper : std::false_type
{};
template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
{};
template <class T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

struct tag_base_of
{
};
struct tag_ref_wrapper
{
};
struct tag_normal
{
};

template <bool MemberFn>
struct class_member_helper
{
    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_base_of, Type T::*f, T1 &&t1, Args &&... args) {
        return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
    }

    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_ref_wrapper, Type T::*f, T1 &&t1, Args &&... args) {
        return (t1.get().*f)(std::forward<Args>(args)...);
    }

    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_normal, Type T::*f, T1 &&t1, Args &&... args) {
        return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
    }
};

template <>
struct class_member_helper<false>
{
    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_base_of, Type T::*f, T1 &&t1, Args &&... args) {
        static_assert(std::is_member_object_pointer<decltype(f)>::value);
        static_assert(sizeof...(args) == 0);
        return std::forward<T1>(t1).*f;
    }

    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_ref_wrapper, Type T::*f, T1 &&t1, Args &&... args) {
        static_assert(std::is_member_object_pointer<decltype(f)>::value);
        static_assert(sizeof...(args) == 0);
        return t1.get().*f;
    }

    template <class T, class Type, class T1, class... Args>
    static decltype(auto) invoke(tag_normal, Type T::*f, T1 &&t1, Args &&... args) {
        static_assert(std::is_member_object_pointer<decltype(f)>::value);
        static_assert(sizeof...(args) == 0);
        return (*std::forward<T1>(t1)).*f;
    }
};

template <class T,
          class T1,
          bool BaseOf = std::is_base_of<T, std::decay_t<T1>>::value,
          bool RefWrapper = is_reference_wrapper_v<std::decay_t<T1>>>
struct member_fn_tag {};

template <class T, class T1>
struct member_fn_tag<T, T1, true, false>
{ using type = tag_base_of; };

template <class T, class T1>
struct member_fn_tag<T, T1, false, true>
{ using type = tag_ref_wrapper; };

template <class T, class T1>
struct member_fn_tag<T, T1, false, false>
{ using type = tag_normal; };

template <class T, class Type, class T1, class... Args>
decltype(auto) INVOKE(Type T::*f, T1 &&t1, Args &&... args) {
    using tag_type = typename member_fn_tag<T, T1>::type;
    return class_member_helper<std::is_member_function_pointer<decltype(f)>::value>::invoke(
        tag_type{}, f, std::forward<T1>(t1), std::forward<Args>(args)...);
}

template <class F, class... Args>
decltype(auto) INVOKE(F &&f, Args &&... args) {
    return std::forward<F>(f)(std::forward<Args>(args)...);
}
} // namespace detail

template <class F, class... Args>
auto invoke(F &&f, Args &&... args) {
    return detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}
} // namespace poly

namespace poly
{
namespace result_helper
{
template <class F, class... Ts>
constexpr auto invoke(F&& f, Ts&&... ts) {
    return poly::invoke(std::forward<F>(f), std::forward<Ts>(ts)...);
}

template <class F, class... Ts>
using invoke_result_t = typename std::result_of<F(Ts...)>::type;

} // namespace result_helper
} // namespace poly
#endif

#if defined(poly_RESULT_NO_OPTIONAL)
#elif defined poly_RESULT_USE_BOOST_OPTIONAL
#include <boost/optional.hpp>
namespace poly
{
template <class T>
using optional_type = boost::optional<T>;
static inline auto nullopt = boost::none;
} // namespace poly
#elif defined(poly_RESULT_USE_OPTIONAL)
#if __has_include (<optional>)
#include <optional>
namespace poly
{
template <class T>
using optional_type = std::optional<T>;
constexpr auto nullopt = std::nullopt;
} // namespace poly
#elif __has_include (<poly/optional.hpp>)
#include <poly/optional.hpp>
namespace poly
{
template<class T>
using optional_type = poly::optional<T>;
}
#elif __has_include (<etl/optional.h>)
#include <etl/optional.h>
namespace poly
{
template <class T>
using optional_type = etl::optional<T>;
constexpr auto nullopt = etl::nullopt;
} // namespace poly
#endif
#else
#include <experimental/optional>
namespace poly
{
template <class T>
using optional_type = std::experimental::optional<T>;
constexpr auto nullopt = std::experimental::nullopt;
} // namespace poly
#endif

//          Copyright Andreas Wass 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

namespace poly
{
namespace result_detail
{
struct no_init_tag
{
};

struct inplace_ok_tag
{
};
struct inplace_error_tag
{
};

template <class... T>
constexpr bool default_constructible_union_v = (poly::is_default_constructible<T>::value && ...);

template <class... T>
constexpr bool trivially_destructible_union_v = (poly::is_trivially_destructible<T>::value && ...);

template <class... T>
constexpr bool move_constructible_union_v = (poly::is_move_constructible<T>::value && ...);

template <class... T>
constexpr bool nothrow_move_constructible_union_v = (poly::is_nothrow_move_constructible<T>::value &&
    ...);

template <class... T>
constexpr bool
    trivially_move_constructible_union_v = (poly::is_trivially_move_constructible<T>::value && ...);

template <class... T>
constexpr bool copy_constructible_union_v = (is_copy_constructible<T>::value && ...);
template <class... T>
constexpr bool nothrow_copy_constructible_union_v = (is_nothrow_copy_constructible<T>::value &&
    ...);

template <class... T>
constexpr bool
    trivially_copy_constructible_union_v = (is_trivially_copy_constructible<T>::value && ...);

template <class... T>
constexpr bool move_assignable_union_v = (is_move_assignable<T>::value && ...);

template <class... T>
constexpr bool nothrow_move_assignable_union_v = (is_nothrow_move_assignable<T>::value && ...);

template <class... T>
constexpr bool trivially_move_assignable_union_v = (is_trivially_move_assignable<T>::value &&
    ...);

template <class... T>
constexpr bool copy_assignable_union_v = (is_copy_assignable<T>::value && ...);

template <class... T>
constexpr bool nothrow_copy_assignable_union_v = (is_nothrow_copy_assignable<T>::value && ...);

template <class... T>
constexpr bool trivially_copy_assignable_union_v = (is_trivially_copy_assignable<T>::value &&
    ...);

template <bool TrivialDestruct>
struct storage_base_tag
{};

template <class T, class E>
using storage_base_tag_for = storage_base_tag<trivially_destructible_union_v<T, E>>;

template <bool MoveConstructible, bool Trivial>
struct move_tag
{};

template <class... T>
using move_tag_for =
move_tag<move_constructible_union_v<T...>, trivially_move_constructible_union_v<T...>>;

template <bool CopyConstructible, bool Trivial>
struct copy_tag
{};

template <class... T>
using copy_tag_for =
copy_tag<copy_constructible_union_v<T...>, trivially_copy_constructible_union_v<T...>>;

template <class T, class E, bool TrivialDestructor = trivially_destructible_union_v<T, E>>
struct result_storage_base;

template <class T, class E>
struct result_storage_base<T, E, true>
{
    union
    {
        T val_;
        E err_;
        char no_init_;
    };
    bool has_val_ = true;

    constexpr result_storage_base() noexcept(is_nothrow_default_constructible<T>::value)
        : val_{}, has_val_{true} {
    }

    constexpr result_storage_base(no_init_tag) noexcept : no_init_{} {
    }

    template <class... Args>
    constexpr result_storage_base(inplace_ok_tag, Args&&... args) noexcept(
    is_nothrow_constructible<T, Args...>::value)
        : val_ {poly::forward<Args>(args)...}, has_val_{true} {
    }

    template <class... Args>
    constexpr result_storage_base(inplace_error_tag, Args&&... args) noexcept(
    is_nothrow_constructible<E, Args...>::value)
        : err_ {poly::forward<Args>(args)...}, has_val_{false} {
    }

    void destroy_current() noexcept {
    }
};

template <class T, class E>
struct result_storage_base<T, E, false>
{
    union
    {
        T val_;
        E err_;
        char dummy_;
    };
    bool has_val_ = true;

    constexpr result_storage_base() noexcept(is_nothrow_default_constructible<T>::value)
        : val_{} {
    }
    constexpr result_storage_base(no_init_tag) noexcept : dummy_{} {
    }

    template <class... Args>
    constexpr result_storage_base(inplace_ok_tag, Args&&... args) noexcept(
    is_nothrow_constructible<T, Args...>::value)
        : val_ {poly::forward<Args>(args)...}, has_val_{true} {
    }

    template <class... Args>
    constexpr result_storage_base(inplace_error_tag, Args&&... args) noexcept(
    is_nothrow_constructible<E, Args...>::value)
        : err_ {poly::forward<Args>(args)...}, has_val_{false} {
    }

    ~result_storage_base() noexcept(is_nothrow_destructible<T>::value &&
                                    is_nothrow_destructible<E>::value) {
        destroy_current();
    }

    void destroy_current() noexcept(is_nothrow_destructible<T>::value &&
                                    is_nothrow_destructible<E>::value) {
        if (has_val_) {
            val_.~T();
        }
        else {
            err_.~E();
        }
    }
};

template<class T, class E, bool DefaultConstructible = default_constructible_union_v<T, E>>
struct result_storage_default_constructor_base: result_storage_base<T, E>
{
    using base_ = result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
};

template<class T, class E>
struct result_storage_default_constructor_base<T, E, false>: result_storage_base<T, E>
{
    using base_ = result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;

    result_storage_default_constructor_base() = delete;
};

template <class T, class E>
struct result_storage_operations_base : result_storage_default_constructor_base<T, E>
{
    using base_ = result_storage_default_constructor_base<T, E>;
    using base_::base_;
    using base_::operator=;

    void copy_construct(const T& t) noexcept(is_nothrow_copy_constructible<T>::value) {
        ::new (poly::addressof(base_::val_)) T(t);
    }
    void copy_construct(const E& e) noexcept(is_nothrow_copy_constructible<E>::value) {
        ::new (poly::addressof(base_::err_)) E(e);
    }
    void move_construct(T&& t) noexcept(is_nothrow_move_constructible<T>::value) {
        ::new (poly::addressof(base_::val_)) T(poly::move(t));
    }
    void move_construct(E&& e) noexcept(is_nothrow_move_constructible<E>::value) {
        ::new (poly::addressof(base_::err_)) E(poly::move(e));
    }
};

template <class T,
    class E,
    bool MoveAssignable = move_assignable_union_v<T, E>,
    bool Trivial = trivially_move_assignable_union_v<T, E>,
    bool MoveConstructible = move_constructible_union_v<T, E>>
struct move_assign_result_storage_base : result_storage_operations_base<T, E>
{
    // This is the case of trivially move assignable
    using base_ = result_storage_operations_base<T, E>;
    using base_::base_;
    using base_::operator=;
};

template <class T, class E>
struct move_assign_result_storage_base<T, E, false, false, false>
    : result_storage_operations_base<T, E>
{
    using base_ = result_storage_operations_base<T, E>;
    using base_::base_;
    using base_::operator=;

    move_assign_result_storage_base(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base(move_assign_result_storage_base&&) = default;

    move_assign_result_storage_base& operator=(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base& operator=(move_assign_result_storage_base&&) = delete;
};

template <class T, class E>
struct move_assign_result_storage_base<T, E, false, false, true>
    : result_storage_operations_base<T, E>
{
    using base_ = result_storage_operations_base<T, E>;
    using base_::base_;
    using base_::operator=;

    move_assign_result_storage_base(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base(move_assign_result_storage_base&&) = default;

    move_assign_result_storage_base& operator=(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base& operator=(move_assign_result_storage_base&& rhs) noexcept(
    noexcept(base_::destroy_current()) &&
    noexcept(base_::move_construct(declval<T>()) &&
             noexcept(base_::move_construct(declval<E>())))) {
        if (this == &rhs) {
            return *this;
        }
        base_::destroy_current();
        base_::has_val_ = rhs.has_val_;
        if (rhs.has_val_) {
            base_::move_construct(move(rhs.val_));
        }
        else {
            base_::move_construct(move(rhs.err_));
        }
        return *this;
    }
};

template <class T, class E>
struct move_assign_result_storage_base<T, E, true, false, true>
    : result_storage_operations_base<T, E>
{
    using base_ = result_storage_operations_base<T, E>;
    using base_::base_;
    using base_::operator=;

    move_assign_result_storage_base(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base(move_assign_result_storage_base&&) = default;

    move_assign_result_storage_base& operator=(const move_assign_result_storage_base&) = default;
    move_assign_result_storage_base& operator=(move_assign_result_storage_base&& rhs) noexcept(
    noexcept(declval<base_>().destroy_current()) && nothrow_move_assignable_union_v<T, E> &&
    noexcept(declval<base_>().move_construct(declval<T>())) &&
    noexcept(declval<base_>().move_construct(declval<E>()))) {
        if (this == &rhs) {
            return *this;
        }
        if (rhs.has_val_ == base_::has_val_) {
            if (rhs.has_val_) {
                base_::val_ =poly::move(rhs.val_);
            }
            else {
                base_::err_ =poly::move(rhs.err_);
            }
        }
        else {
            base_::destroy_current();
            base_::has_val_ = rhs.has_val_;
            if (rhs.has_val_) {
                base_::move_construct(poly::move(rhs.val_));
            }
            else {
                base_::move_construct(poly::move(rhs.err_));
            }
        }
        return *this;
    }
};

template <class T,
    class E,
    bool MoveConstructible = move_constructible_union_v<T, E>,
    bool Trivial = trivially_move_constructible_union_v<T, E>>
struct move_result_storage_base;

template <class T, class E>
struct move_result_storage_base<T, E, true, false> : move_assign_result_storage_base<T, E>
{
    using base_ = move_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    move_result_storage_base(move_result_storage_base&& rhs) noexcept(
    is_nothrow_move_constructible<T>::value &&
    is_nothrow_move_constructible<E>::value)
        : base_{no_init_tag{}} {
        base_::has_val_ = rhs.has_val_;
        if (rhs.has_val_) {
            ::new (poly::addressof(base_::val_)) T{poly::move(rhs.val_)};
        }
        else {
            ::new (poly::addressof(base_::err_)) E{poly::move(rhs.err_)};
        }
    }
    move_result_storage_base(const move_result_storage_base&) = default;

    move_result_storage_base& operator=(move_result_storage_base&& rhs) = default;
    move_result_storage_base& operator=(const move_result_storage_base&) = default;
};

template <class T, class E>
struct move_result_storage_base<T, E, false, false> : move_assign_result_storage_base<T, E>
{
    using base_ = move_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    move_result_storage_base(move_result_storage_base&&) = delete;
    move_result_storage_base(const move_result_storage_base&) = default;

    move_result_storage_base& operator=(move_result_storage_base&&) = delete;
    move_result_storage_base& operator=(const move_result_storage_base&) = default;
};

template <class T, class E>
struct move_result_storage_base<T, E, true, true> : move_assign_result_storage_base<T, E>
{
    using base_ = move_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    move_result_storage_base(move_result_storage_base&&) = default;
    move_result_storage_base(const move_result_storage_base&) = default;

    move_result_storage_base& operator=(move_result_storage_base&&) = default;
    move_result_storage_base& operator=(const move_result_storage_base&) = default;
};

template <class T,
    class E,
    bool CopyAssignable = copy_assignable_union_v<T, E>,
    bool Trivial = trivially_copy_assignable_union_v<T, E>,
    bool CopyConstructible = copy_constructible_union_v<T, E>>
struct copy_assign_result_storage_base : move_result_storage_base<T, E>
{
    // This is the case of trivially copy assignable
    using base_ = move_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
};

template <class T, class E>
struct copy_assign_result_storage_base<T, E, false, false, false> : move_result_storage_base<T, E>
{
    using base_ = move_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;

    copy_assign_result_storage_base(const copy_assign_result_storage_base&) = default;
    copy_assign_result_storage_base(copy_assign_result_storage_base&&) = default;

    copy_assign_result_storage_base& operator=(const copy_assign_result_storage_base&) = delete;
    copy_assign_result_storage_base& operator=(copy_assign_result_storage_base&&) = default;
};

template <class T, class E>
struct copy_assign_result_storage_base<T, E, false, false, true> : move_result_storage_base<T, E>
{
    using base_ = move_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;

    copy_assign_result_storage_base(const copy_assign_result_storage_base&) = default;
    copy_assign_result_storage_base(copy_assign_result_storage_base&&) = default;

    copy_assign_result_storage_base& operator=(copy_assign_result_storage_base&&) = default;
    copy_assign_result_storage_base& operator=(const copy_assign_result_storage_base& rhs) noexcept(
    noexcept(base_::destroy_current()) && noexcept(base_::copy_construct(declval<T>())) &&
    noexcept(base_::copy_construct(declval<E>()))) {
        if (this == &rhs) {
            return *this;
        }
        base_::destroy_current();
        base_::has_val_ = rhs.has_val_;
        if (rhs.has_val_) {
            base_::copy_construct(rhs.val_);
        }
        else {
            base_::copy_construct(rhs.err_);
        }
        return *this;
    }
};

template <class T, class E>
struct copy_assign_result_storage_base<T, E, true, false, true> : move_result_storage_base<T, E>
{
    using base_ = move_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;

    copy_assign_result_storage_base(const copy_assign_result_storage_base&) = default;
    copy_assign_result_storage_base(copy_assign_result_storage_base&&) = default;

    copy_assign_result_storage_base& operator=(copy_assign_result_storage_base&&) = default;
    copy_assign_result_storage_base& operator=(const copy_assign_result_storage_base& rhs) noexcept(
    noexcept(base_::destroy_current()) && nothrow_copy_constructible_union_v<T, E> &&
    nothrow_copy_constructible_union_v<T, E>) {
        if (this == &rhs) {
            return *this;
        }
        if (rhs.has_val_ == base_::has_val_) {
            if (rhs.has_val_) {
                base_::val_ = rhs.val_;
            }
            else {
                base_::err_ = rhs.err_;
            }
        }
        else {
            base_::destroy_current();
            base_::has_val_ = rhs.has_val_;
            if (rhs.has_val_) {
                this->copy_construct(rhs.val_);
            }
            else {
                this->copy_construct(rhs.err_);
            }
        }
        return *this;
    }
};

template <class T,
    class E,
    bool Copyable = copy_constructible_union_v<T, E>,
    bool Trivial = trivially_copy_constructible_union_v<T, E>>
struct copy_result_storage_base;

template <class T, class E>
struct copy_result_storage_base<T, E, true, false> : copy_assign_result_storage_base<T, E>
{
    using base_ = copy_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    copy_result_storage_base(const copy_result_storage_base& rhs) noexcept(
    nothrow_copy_constructible_union_v<T, E>)
        : base_{no_init_tag{}} {
        base_::has_val_ = rhs.has_val_;
        if (rhs.has_val_) {
            ::new (poly::addressof(base_::val_)) T{rhs.val_};
        }
        else {
            ::new (poly::addressof(base_::err_)) E{rhs.err_};
        }
    }
    copy_result_storage_base(copy_result_storage_base&&) = default;

    copy_result_storage_base& operator=(const copy_result_storage_base& rhs) = default;
    copy_result_storage_base& operator=(copy_result_storage_base&&) = default;
};

template <class T, class E>
struct copy_result_storage_base<T, E, false, false> : copy_assign_result_storage_base<T, E>
{
    using base_ = copy_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    copy_result_storage_base(const copy_result_storage_base&) = delete;
    copy_result_storage_base(copy_result_storage_base&&) = default;

    copy_result_storage_base& operator=(const copy_result_storage_base&) = delete;
    copy_result_storage_base& operator=(copy_result_storage_base&&) = default;
};

template <class T, class E>
struct copy_result_storage_base<T, E, true, true> : copy_assign_result_storage_base<T, E>
{
    using base_ = copy_assign_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;
    copy_result_storage_base(const copy_result_storage_base&) = default;
    copy_result_storage_base(copy_result_storage_base&&) = default;

    copy_result_storage_base& operator=(const copy_result_storage_base&) = default;
    copy_result_storage_base& operator=(copy_result_storage_base&&) = default;
};

template <class T, class E>
struct result_storage : copy_result_storage_base<T, E>
{
    using base_ = copy_result_storage_base<T, E>;
    using base_::base_;
    using base_::operator=;

    using value_type = T;
    using error_type = E;

    void destroy_current() noexcept(
    noexcept(declval<base_>().destroy_current())
    ) {
        base_::destroy_current();
    }

    T* value_ptr() noexcept {
        return LAUNDER(poly::addressof(base_::val_));
    }

    const T* value_ptr() const noexcept {
        return LAUNDER(poly::addressof(base_::val_));
    }

    E* error_ptr() noexcept {
        return LAUNDER(poly::addressof(base_::err_));
    }

    const E* error_ptr() const noexcept {
        return LAUNDER(poly::addressof(base_::err_));
    }

    NODISCARD constexpr bool has_val() const {
        return base_::has_val_;
    }
};
} // namespace result_detail
} // namespace poly

//          Copyright Andreas Wass 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

namespace poly
{
namespace result_detail
{
struct monostate
{
};

template <class TypeT, bool DefaultConstructible = is_default_constructible<TypeT>::value>
struct default_constructible_base
{};

template <class TypeT>
struct default_constructible_base<TypeT, false>
{
    default_constructible_base() = delete;
    default_constructible_base(no_init_tag) noexcept {}
    default_constructible_base(const default_constructible_base&) = default;
    default_constructible_base(default_constructible_base&&) noexcept = default;

    default_constructible_base& operator=(const default_constructible_base&) = default;
    default_constructible_base& operator=(default_constructible_base&&) noexcept = default;
};

template <class TypeT, bool Constructible = is_copy_constructible<TypeT>::value>
struct copy_constructible_base
{};

template <class TypeT>
struct copy_constructible_base<TypeT, false>
{
    copy_constructible_base() = default;
    copy_constructible_base(const copy_constructible_base&) = delete;
    copy_constructible_base(copy_constructible_base&&) noexcept = default;

    copy_constructible_base& operator=(const copy_constructible_base&) = default;
    copy_constructible_base& operator=(copy_constructible_base&&) noexcept = default;
};

template <class TypeT, bool Constructible = is_move_constructible<TypeT>::value>
struct move_constructible_base
{};

template <class TypeT>
struct move_constructible_base<TypeT, false>
{
    move_constructible_base() = default;
    move_constructible_base(const move_constructible_base&) = default;
    move_constructible_base(move_constructible_base&&) noexcept = delete;

    move_constructible_base& operator=(const move_constructible_base&) = default;
    move_constructible_base& operator=(move_constructible_base&&) noexcept = default;
};

template <class TypeT>
struct ok_tag : default_constructible_base<TypeT>,
                copy_constructible_base<TypeT>,
                move_constructible_base<TypeT>
{
    using default_constructible_base<TypeT>::default_constructible_base;
    using copy_constructible_base<TypeT>::copy_constructible_base;
    using move_constructible_base<TypeT>::move_constructible_base;

    using value_type = TypeT;
    using reference = TypeT&;
    using const_reference = const TypeT&;
    using rvalue_reference = TypeT&&;
    value_type value;

    ok_tag() = default;
    ok_tag(const ok_tag&) = default;
    ok_tag(ok_tag&&) = default;

    ok_tag& operator=(const ok_tag&) = default;
    ok_tag& operator=(ok_tag&&) = default;
    ~ok_tag() = default;

    template <class V,
        enable_if_t<is_constructible<TypeT, V>::value &&
                            is_default_constructible<TypeT>::value &&
                         !is_same<decay_t<V>, ok_tag<TypeT>>::value>* = nullptr>
    ok_tag(V&& v) noexcept : value(poly::forward<V>(v)) {
    }

    template <class V,
            enable_if_t<is_constructible<TypeT, V>::value &&
                               !is_default_constructible<TypeT>::value &&
                               !is_same<decay_t<V>, ok_tag<TypeT>>::value>* = nullptr>
    ok_tag(V&& v) noexcept : default_constructible_base<TypeT>(result_detail::no_init_tag{}), value(poly::forward<V>(v)) {
    }
};

template <>
struct ok_tag<void>
{
    using value_type = monostate;
    using reference = monostate;
    using const_reference = monostate;
    using rvalue_reference = monostate;
    value_type value;
};

template <class T>
struct ok_traits;

template <class T>
struct ok_traits<ok_tag<T>>
{ using type = T; };

template <class T>
struct is_ok_tag : public false_type
{};

template <class T>
struct is_ok_tag<ok_tag<T>> : public true_type
{};

template <class TypeT>
struct error_tag
{
    using value_type = TypeT;
    using reference = TypeT&;
    using const_reference = const TypeT&;
    value_type value;

    error_tag() = default;
    template <class V, enable_if_t<is_constructible<value_type, V>::value>* = nullptr>
    error_tag(V&& v) noexcept : value(poly::forward<V>(v)) {
    }
};

template <class T>
struct is_error_tag : public false_type
{};

template <class T>
struct is_error_tag<error_tag<T>> : public true_type
{};

template <class T>
struct error_traits;

template <class T>
struct error_traits<error_tag<T>>
{ using type = T; };

} // namespace result_detail
} // namespace poly

namespace poly
{

template <class T, class E>
class result;

/**
 * @brief Creates an ok value containing the specified value
 * @param value The ok value
 * @returns An ok containing `value`
 */
template <class T>
NODISCARD result_detail::ok_tag<decay_t<T>> ok(T&& value) {
    return result_detail::ok_tag<decay_t<T>> {poly::forward<T>(value)};
}
/**
 * @brief Creates a void ok value
 * @returns A void ok value
 */
NODISCARD inline result_detail::ok_tag<void> ok() {
    return result_detail::ok_tag<void>{};
}

/**
 * @brief Creates an error containing the specified value
 * @param err The error value
 * @returns An error containing `err`
 */
template <class E>
NODISCARD result_detail::error_tag<decay_t<E>> error(E&& err) {
    return result_detail::error_tag<decay_t<E>> {poly::forward<E>(err)};
}

namespace result_detail
{
template <class T>
struct is_result : false_type
{};

template <class T, class E>
struct is_result<result<T, E>> : true_type
{};

template <class T>
struct result_traits;

template <class T, class E>
struct result_traits<result<T, E>>
{};

template <class S, class DS = decay_t<S>>
static constexpr bool is_void_value_v = is_same<typename DS::value_type, ok_tag<void>>::value;

template <class S, class DS = decay_t<S>>
using storage_value_t =
typename conditional<is_void_value_v<S>, void, typename DS::value_type::value_type>::type;

template <class S, class DS = decay_t<S>>
using storage_error_t = typename DS::error_type::value_type;

template <class F, class S, bool IsVoidValue = is_void_value_v<S>>
struct invoke_with_value_result
{
    using value_type_ = typename decay_t<S>::value_type::value_type;
    using reference_type = typename conditional<is_rvalue_reference_v<S>, value_type_&&, value_type_&>::type;
    using const_qualified_ref_ = typename conditional<is_const_v<S>, const reference_type, reference_type>::type;
    using type = decay_t<
        result_helper::invoke_result_t<F, const_qualified_ref_>>;
};

template <class F, class S>
struct invoke_with_value_result<F, S, true>
{ using type = decay_t<result_helper::invoke_result_t<F>>; };

template <class F, class S>
using invoke_with_value_result_t = typename invoke_with_value_result<F, S>::type;

template <class F, class S>
using invoke_with_error_result_t =
decay_t<result_helper::invoke_result_t<F, storage_error_t<S>>>;

template <class S>
using disable_for_void_value = enable_if_t<!is_void_value_v<S>>;

template <class S>
using enable_for_void_value = enable_if_t<is_void_value_v<S>>;

template <class Function, class S, disable_for_void_value<S>* = nullptr>
constexpr auto invoke_with_ok_value(Function&& f, S&& s) {
    return result_helper::invoke(poly::forward<Function>(f), (poly::forward<S>(s).val_.value));
}

template <class Function, class S>
constexpr auto invoke_with_error_value(Function&& f, S&& s) {
    return result_helper::invoke(poly::forward<Function>(f), (poly::forward<S>(s).err_.value));
}

template <class Function, class S, enable_for_void_value<S>* = nullptr>
constexpr auto invoke_with_ok_value(Function&& f, S&& s) {
    (void)s;
    return f();
}

template <class F,
    class S,
    class Ret = invoke_with_value_result_t<F, S>,
    disable_for_void_value<S>* = nullptr,
    enable_if_t<!is_void<Ret>::value>* = nullptr>
constexpr auto map_impl(F&& f, S&& s) {
    using ret_type = result<Ret, storage_error_t<S>>;
    if (s.has_val()) {
        return ret_type{ok(invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s)))};
    }
    return ret_type{poly::error(poly::forward<S>(s).err_.value)};
}
template <class F,
    class S,
    class Ret = invoke_with_value_result_t<F, S>,
    disable_for_void_value<S>* = nullptr,
    enable_if_t<is_void<Ret>::value>* = nullptr>
constexpr auto map_impl(F&& f, S&& s) {
    using ret_type = result<Ret, storage_error_t<S>>;
    if (s.has_val()) {
        invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s));
        return ret_type{ok()};
    }
    return ret_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = decltype(declval<F>()()),
    enable_for_void_value<S>* = nullptr,
    enable_if_t<!is_void<Ret>::value>* = nullptr>
constexpr auto map_impl(F&& f, S&& s) {
    using ret_type = result<Ret, storage_error_t<S>>;
    if (s.has_val()) {
        return ret_type{ok(f())};
    }
    return ret_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = decltype(declval<F>()()),
    enable_for_void_value<S>* = nullptr,
    enable_if_t<is_void<Ret>::value>* = nullptr>
constexpr auto map_impl(F&& f, S&& s) {
    using ret_type = result<Ret, storage_error_t<S>>;
    if (s.has_val()) {
        f();
        return ret_type{ok()};
    }
    return ret_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class Fallback,
    class S,
    class Ret = decltype(invoke_with_ok_value(declval<F>(), declval<S>()))>
constexpr auto map_or_else_impl(F&& f, Fallback&& fb, S&& s) {
    using error_return_type = invoke_with_error_result_t<Fallback, S>;
    static_assert(is_same<Ret, error_return_type>::value ||
                  is_convertible<error_return_type, Ret>::value,
                  "fallback return type must be at least convertible to the mapped return type");
    if (s.has_val()) {
        return invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s));
    }
    return Ret{invoke_with_error_value(poly::forward<Fallback>(fb), poly::forward<S>(s))};
}

template <class F,
    class S,
    class Ret = decltype(declval<F>()(declval<storage_error_t<S>>())),
    enable_if_t<!is_void<decay_t<Ret>>::value>* = nullptr>
constexpr auto map_error_impl(F&& f, S&& s) {
    using return_type = result<storage_value_t<S>, Ret>;
    if (s.has_val()) {
        return return_type{poly::ok(poly::forward<S>(s).val_.value)};
    }
    return return_type{
        poly::error(invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s)))};
}

template <class F,
    class S,
    class Ret = invoke_with_value_result_t<F, S>,
    enable_if_t<!is_ok_tag<Ret>::value && !is_error_tag<Ret>::value>* = nullptr,
    disable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    static_assert(is_same<typename Ret::error_type, storage_error_t<S>>::value,
                  "Error type mismatch");
    if (s.has_val()) {
        return invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s));
    }
    return Ret{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = result_helper::invoke_result_t<F>,
    enable_if_t<!is_ok_tag<Ret>::value && !is_error_tag<Ret>::value>* = nullptr,
    enable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    static_assert(is_same<typename Ret::error_type, storage_error_t<S>>::value,
                  "Error type mismatch");
    if (s.has_val()) {
        return f();
    }
    return Ret{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = invoke_with_value_result_t<F, S>,
    enable_if_t<is_ok_tag<Ret>::value>* = nullptr,
    disable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    using result_type = result<typename ok_traits<Ret>::type, storage_error_t<S>>;
    if (s.has_val()) {
        return result_type(invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s)));
    }
    return result_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = result_helper::invoke_result_t<F>,
    enable_if_t<is_ok_tag<Ret>::value>* = nullptr,
    enable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    using result_type = result<typename ok_traits<Ret>::type, storage_error_t<S>>;
    if (s.has_val()) {
        return result_type{f()};
    }
    return result_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = invoke_with_value_result_t<F, S>,
    enable_if_t<is_error_tag<Ret>::value>* = nullptr,
    disable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    static_assert(is_same<typename decay_t<Ret>::value_type, storage_error_t<S>>::value,
                  "Error type mismatch");
    using result_type = result<storage_value_t<S>, storage_error_t<S>>;
    if (s.has_val()) {
        return result_type(invoke_with_ok_value(poly::forward<F>(f), poly::forward<S>(s)));
    }
    return result_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = result_helper::invoke_result_t<F>,
    enable_if_t<is_error_tag<Ret>::value>* = nullptr,
    enable_for_void_value<S>* = nullptr>
constexpr auto and_then_impl(F&& f, S&& s) {
    using result_type = result<storage_value_t<S>, storage_error_t<S>>;
    if (s.has_val()) {
        return result_type{f()};
    }
    return result_type{poly::error(poly::forward<S>(s).err_.value)};
}

template <class F,
    class S,
    class Ret = invoke_with_error_result_t<F, S>,
    enable_if_t<!is_ok_tag<Ret>::value && !is_error_tag<Ret>::value &&
                       poly::is_invocable_v<F, storage_error_t<S>>
    >* = nullptr>
constexpr auto or_else_impl(F&& f, S&& s) {
    if constexpr (is_same<Ret, void>::value)
    {
        using result_type = result<storage_value_t<S>, storage_error_t<S>>;
        if(!s.has_val()) {
            invoke_with_error_value(poly::forward<F>(f), s);
            return result_type{poly::error(poly::forward<S>(s).err_.value)};
        }
        return result_type{poly::ok(poly::forward<S>(s).val_.value)};
    }
    else
    {
        static_assert(is_result<Ret>::value, "Function must return a result or an ok()");
        static_assert(is_same<typename Ret::value_type, storage_value_t<S>>::value,
                      "Value type mismatch");
        if (!s.has_val()) {
            return invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s));
        }
        return Ret{poly::ok(poly::forward<S>(s).val_.value)};
    }
}

template <class F,
        class S,
        class Ret = result_helper::invoke_result_t<F>,
        enable_if_t<!is_ok_tag<Ret>::value && !is_error_tag<Ret>::value && poly::is_invocable_v<F> &&
                    !poly::is_invocable_v<F, storage_error_t<S>>
        >* = nullptr>
constexpr auto or_else_impl(F&& f, S&& s) {
    if constexpr (is_same<Ret, void>::value)
    {
        using result_type = result<storage_value_t<S>, storage_error_t<S>>;
        if(!s.has_val()) {
            f();
            return result_type{poly::error(poly::forward<S>(s).err_.value)};
        }
        return result_type{poly::ok(poly::forward<S>(s).val_.value)};
    }
    else
    {
        static_assert(is_result<Ret>::value, "Function must return a result");
        static_assert(is_same<typename Ret::value_type, storage_value_t<S>>::value,
                      "Value type mismatch");
        if (!s.has_val()) {
            return Ret{f()};
        }
        return Ret{poly::ok(poly::forward<S>(s).val_.value)};
    }
}

template <class F,
    class S,
    class Ret = invoke_with_error_result_t<F, S>,
    enable_if_t<is_ok_tag<Ret>::value>* = nullptr>
constexpr auto or_else_impl(F&& f, S&& s) {
    static_assert(is_convertible<typename ok_traits<Ret>::type, storage_value_t<S>>::value,
                  "or_else must not change ok type to some non-convertible type");
    using result_type = result<storage_value_t<S>, storage_error_t<S>>;
    if (!s.has_val()) {
        return result_type{
            result_detail::invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s))};
    }
    return result_type{poly::ok(poly::forward<S>(s).val_.value)};
}

template <class F,
    class S,
    class Ret = invoke_with_error_result_t<F, S>,
    enable_if_t<is_error_tag<Ret>::value>* = nullptr>
constexpr auto or_else_impl(F&& f, S&& s) {
    using result_type = result<storage_value_t<S>, typename error_traits<Ret>::type>;
    if (!s.has_val()) {
        return result_type{invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s))};
    }
    return result_type{poly::ok(poly::forward<S>(s).val_.value)};
}

template <class F,
    class S,
    class Ret = invoke_with_error_result_t<F, S>,
    disable_for_void_value<S>* = nullptr>
constexpr auto unwrap_or_else_impl(F&& f, S&& s) {
    static_assert(is_same<Ret, storage_value_t<S>>::value, "Value type mismatch");
    if (!s.has_val()) {
        return invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s));
    }
    return poly::forward<S>(s).val_.value;
}

template <class F,
    class S,
    class Ret = invoke_with_error_result_t<F, S>,
    enable_for_void_value<S>* = nullptr>
constexpr void unwrap_or_else_impl(F&& f, S&& s) {
    static_assert(is_same<Ret, storage_value_t<S>>::value, "Value type mismatch");
    if (!s.has_val()) {
        invoke_with_error_value(poly::forward<F>(f), poly::forward<S>(s));
    }
}

template <class S, class U, disable_for_void_value<S>* = nullptr>
constexpr storage_value_t<S> unwrap_or_impl(U&& u, S&& s) {
    if (!s.has_val()) {
        return poly::forward<U>(u);
    }
    return poly::forward<S>(s).val_.value;
}

struct result_comparator
{
    template <class T, class T2>
    static bool equal(const T& lhs, const T2& rhs) {
        if (lhs.is_ok() == rhs.is_ok()) {
            if (lhs.is_ok()) {
                return lhs.storage_.val_.value == rhs.storage_.val_.value;
            }
            return lhs.storage_.err_.value == rhs.storage_.err_.value;
        }
        return false;
    }

    template <class E, class E2>
    static bool equal(const poly::result<void, E>& lhs, const poly::result<void, E2>& rhs) {
        if (lhs.is_ok() == rhs.is_ok()) {
            if (lhs.is_ok()) {
                return true;
            }
            return lhs.storage_.err_.value == rhs.storage_.err_.value;
        }
        return false;
    }
};

} // namespace result_detail

/**
 * @brief A result-type inspired by Rusts Result type
 * @tparam T The type of _ok_ value stored by this result
 * @tparam E The type of _error_ value stored by this result
 */
template <class T, class E>
class result
{
    using storage_type = result_detail::result_storage<result_detail::ok_tag<T>, result_detail::error_tag<E>>;
public:
    static_assert(!is_reference<T>::value, "T must not be a reference type");
    static_assert(!is_reference<E>::value, "E must not be a reference type");

    using value_type = T;
    using error_type = E;
    using optional_value_type = typename result_detail::ok_tag<T>::value_type;

    result() = default;
    result(const result&) = default;
    result(result&&) = default;

    result& operator=(const result&) = default;
    result& operator=(result&&) = default;

    result(result_detail::ok_tag<T>&& rhs) noexcept
        : storage_(result_detail::inplace_ok_tag{},poly::move(rhs)) {
    }
    result(result_detail::error_tag<E>&& rhs) noexcept
        : storage_(result_detail::inplace_error_tag{},poly::move(rhs)) {
    }

    result(const result_detail::ok_tag<T>& rhs) noexcept
        : storage_(result_detail::inplace_ok_tag{}, rhs) {
    }
    result(const result_detail::error_tag<E>& rhs) noexcept
        : storage_(result_detail::inplace_error_tag{}, rhs) {
    }

    // Conversion constructors
    template <class U,
        class F,
        enable_if_t<is_convertible<U, T>::value &&
                         is_convertible<F, E>::value>* = nullptr>
    explicit result(const result<U, F>& rhs) noexcept(
    is_nothrow_constructible<result_detail::ok_tag<T>,
        const result_detail::ok_tag<U>&>::value&&
    is_nothrow_constructible<result_detail::error_tag<E>,
        const result_detail::error_tag<F>&>::value)
        : storage_(result_detail::no_init_tag{}) {
        storage_.has_val_ = rhs.storage_.has_val_;
        if (rhs.is_ok()) {
            ::new (storage_.value_ptr()) result_detail::ok_tag<T>{rhs.storage_.val_.value};
        }
        else {
            ::new (storage_.error_ptr()) result_detail::error_tag<E>{rhs.storage_.err_.value};
        }
    }

    template <class U,
        class F,
        enable_if_t<is_convertible<U, T>::value &&
                         is_convertible<F, E>::value>* = nullptr>
    result& operator=(const result<U, F>& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::ok_tag<T>,
        const result_detail::ok_tag<U>&>::value &&
    is_nothrow_constructible<result_detail::error_tag<E>,
        const result_detail::error_tag<F>&>::value) {
        if (rhs.is_ok() == is_ok()) {
            if (is_ok()) {
                storage_.value_ptr()->value = rhs.storage_.value_ptr()->value;
            }
            else {
                storage_.error_ptr()->value = rhs.storage_.error_ptr()->value;
            }
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = rhs.storage_.has_val_;
            if (rhs.is_ok()) {
                ::new (storage_.value_ptr()) result_detail::ok_tag<T>{rhs.storage_.val_.value};
            }
            else {
                ::new (storage_.error_ptr()) result_detail::error_tag<E>{rhs.storage_.err_.value};
            }
        }
        return *this;
    }

    template <class U,
        class F,
        enable_if_t<is_convertible<U, T>::value &&
                         is_convertible<F, E>::value>* = nullptr>
    explicit result(result<U, F>&& rhs) noexcept(
    is_nothrow_constructible<result_detail::ok_tag<T>, result_detail::ok_tag<U>&&>::value&&
    is_nothrow_constructible<result_detail::error_tag<E>,
        result_detail::error_tag<F>&&>::value)
        : storage_(result_detail::no_init_tag{}) {
        storage_.has_val_ = rhs.storage_.has_val_;
        if (rhs.is_ok()) {
            ::new (storage_.value_ptr())
                result_detail::ok_tag<T>{poly::move(rhs).storage_.val_.value};
        }
        else {
            ::new (storage_.error_ptr())
                result_detail::error_tag<E>{poly::move(rhs).storage_.err_.value};
        }
    }

    template <class U,
        class F,
        enable_if_t<is_convertible<U, T>::value &&
                         is_convertible<F, E>::value>* = nullptr>
    result& operator=(result<U, F>&& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::ok_tag<T>,
        result_detail::ok_tag<U>&&>::value &&
    is_nothrow_constructible<result_detail::error_tag<E>,
        result_detail::error_tag<F>&&>::value) {
        if (rhs.is_ok() == is_ok()) {
            if (is_ok()) {
                storage_.value_ptr()->value =poly::move(rhs.storage_.value_ptr()->value);
            }
            else {
                storage_.error_ptr()->value =poly::move(rhs.storage_.error_ptr()->value);
            }
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = rhs.storage_.has_val_;
            if (rhs.is_ok()) {
                ::new (storage_.value_ptr())
                    result_detail::ok_tag<T>{poly::move(rhs).storage_.val_.value};
            }
            else {
                ::new (storage_.error_ptr())
                    result_detail::error_tag<E>{poly::move(rhs).storage_.err_.value};
            }
        }
        return *this;
    }

    template <class U,
        enable_if_t<is_convertible<U, T>::value || is_same<T, void>::value>* =
        nullptr>
    result(result_detail::ok_tag<U>&& rhs) noexcept
        : storage_(result_detail::inplace_ok_tag{},poly::move(rhs.value)) {
    }

    template <class U,
        enable_if_t<is_convertible<U, T>::value || is_same<T, void>::value>* =
        nullptr>
    result(const result_detail::ok_tag<U>& rhs) noexcept
        : storage_(result_detail::inplace_ok_tag{}, rhs.value) {
    }

    template <class F, enable_if_t<is_convertible<F, E>::value>* = nullptr>
    result(result_detail::error_tag<F>&& rhs) noexcept
        : storage_(result_detail::inplace_error_tag{},poly::move(rhs.value)) {
    }

    template <class F, enable_if_t<is_convertible<F, E>::value>* = nullptr>
    result(const result_detail::error_tag<F>& rhs) noexcept
        : storage_(result_detail::inplace_error_tag{}, rhs.value) {
    }

    // Conversion operators
    template <class U, enable_if_t<is_convertible<U, T>::value>* = nullptr>
    result& operator=(const result_detail::ok_tag<U>& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::ok_tag<T>,
        const result_detail::ok_tag<U>&>::value) {
        if (is_ok()) {
            storage_.value_ptr()->value = rhs.value;
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = true;
            ::new (storage_.value_ptr()) result_detail::ok_tag<T>{rhs.value};
        }
        return *this;
    }

    template <class U, enable_if_t<is_convertible<U, T>::value>* = nullptr>
    result& operator=(result_detail::ok_tag<U>&& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::ok_tag<T>,
        result_detail::ok_tag<U>&&>::value) {
        if (is_ok()) {
            storage_.value_ptr()->value =poly::move(rhs.value);
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = true;
            ::new (storage_.value_ptr()) result_detail::ok_tag<T>{poly::move(rhs.value)};
        }
        return *this;
    }

    template <class F, enable_if_t<is_convertible<F, E>::value>* = nullptr>
    result& operator=(const result_detail::error_tag<F>& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::error_tag<E>,
        const result_detail::error_tag<F>&>::value) {
        if (is_error()) {
            storage_.error_ptr()->value = rhs.value;
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = false;
            ::new (storage_.error_ptr()) result_detail::error_tag<E>{rhs.value};
        }
        return *this;
    }

    template <class F, enable_if_t<is_convertible<F, E>::value>* = nullptr>
    result& operator=(result_detail::error_tag<F>&& rhs) noexcept(
    noexcept(declval<storage_type>().destroy_current()) &&
    is_nothrow_constructible<result_detail::error_tag<E>,
        result_detail::error_tag<F>&&>::value) {
        if (is_error()) {
            storage_.error_ptr()->value =poly::move(rhs.value);
        }
        else {
            storage_.destroy_current();
            storage_.has_val_ = false;
            ::new (storage_.error_ptr()) result_detail::error_tag<E>{poly::move(rhs.value)};
        }
        return *this;
    }

    /**
     * @brief Returns true if the result holds an ok() value
     * @returns True if result is ok
     */
    NODISCARD bool is_ok() const noexcept {
        return storage_.has_val();
    }

    /**
     * @brief Returns true if the result holds an error value
     * @returns True if result is an error
     */
    NODISCARD bool is_error() const noexcept {
        return !is_ok();
    }

#if !defined(poly_RESULT_NO_OPTIONAL)
    /**
     * @brief Optionally get an ok value.
     * @returns The stored ok() value if one is stored, otherwise an optional nullopt
     */
    NODISCARD optional_type<optional_value_type> maybe_value() & noexcept(
    noexcept(optional_type<optional_value_type>(declval<storage_type>().val_.value))) {
        if (is_ok()) {
            return storage_.val_.value;
        }
        return poly::nullopt;
    }
    NODISCARD optional_type<optional_value_type> maybe_value() const& noexcept(
    noexcept(optional_type<optional_value_type>(declval<storage_type>().val_.value))) {
        if (is_ok()) {
            return storage_.val_.value;
        }
        return poly::nullopt;
    }
    NODISCARD optional_type<optional_value_type> maybe_value() && noexcept(
    noexcept(optional_type<optional_value_type>(move(declval<storage_type>()).val_.value))) {
        if (is_ok()) {
            return poly::move(storage_).val_.value;
        }
        return poly::nullopt;
    }
    NODISCARD optional_type<optional_value_type> maybe_value() const&& noexcept(
    noexcept(optional_type<optional_value_type>(move(declval<storage_type>()).val_.value))) {
        if (is_ok()) {
            return poly::move(storage_).val_.value;
        }
        return poly::nullopt;
    }

    /**
     * @brief Get the stored error value, or an optional nullopt if `is_ok() == true`
     * @returns Either a stored error value or a nullopt
     */
    NODISCARD optional_type<error_type>
    maybe_error() & noexcept(noexcept(optional_type<error_type>(declval<storage_type>().err_.value))) {
        if (is_ok()) {
            return poly::nullopt;
        }
        return storage_.err_.value;
    }
    NODISCARD optional_type<error_type>
    maybe_error() const& noexcept(noexcept(optional_type<error_type>(declval<storage_type>().err_.value))) {
        if (is_ok()) {
            return poly::nullopt;
        }
        return storage_.err_.value;
    }
    NODISCARD optional_type<error_type> maybe_error() && noexcept(
    noexcept(optional_type<error_type>(move(declval<storage_type>()).err_.value))) {
        if (is_ok()) {
            return poly::nullopt;
        }
        return poly::move(storage_).err_.value;
    }
    NODISCARD optional_type<error_type> maybe_error() const&& noexcept(
    noexcept(optional_type<error_type>(move(declval<storage_type>()).err_.value))) {
        if (is_ok()) {
            return poly::nullopt;
        }
        return poly::move(storage_).err_.value;
    }
#endif

    /**
     * @brief Apply a mapping function to an ok() value
     * @param mapper A callable mapping `T -> U`
     * @returns A result<U, E>
     *
     * Maps a `result<T, E>` to `result<U, E>` by applying a function to a contained ok
     * value. An error will be left untouched and returned in the result.
     */
    template <class FunctionT>
    NODISCARD auto map(FunctionT&& mapper) & noexcept {
        return result_detail::map_impl(poly::forward<FunctionT>(mapper), storage_);
    }
    template <class FunctionT>
    NODISCARD auto map(FunctionT&& mapper) const& noexcept {
        return result_detail::map_impl(poly::forward<FunctionT>(mapper), storage_);
    }
    template <class FunctionT>
    NODISCARD auto map(FunctionT&& mapper) && noexcept {
        return result_detail::map_impl(poly::forward<FunctionT>(mapper),poly::move(storage_));
    }
    template <class FunctionT>
    NODISCARD auto map(FunctionT&& mapper) const&& noexcept {
        return result_detail::map_impl(poly::forward<FunctionT>(mapper),poly::move(storage_));
    }

    /**
     * @brief Maps a `result<T, E>` to `result<T, F>` by applying a mapper to a contained error.
     * @param mapper A callable mapping `E -> F`
     * @returns A `result<T, F>` with either an untouched ok, or a mapped error.
     */
    template <class FunctionT>
    NODISCARD auto map_error(FunctionT&& mapper) & noexcept {
        return result_detail::map_error_impl(poly::forward<FunctionT>(mapper), storage_);
    }
    template <class FunctionT>
    NODISCARD auto map_error(FunctionT&& mapper) const& noexcept {
        return result_detail::map_error_impl(poly::forward<FunctionT>(mapper), storage_);
    }
    template <class FunctionT>
    NODISCARD auto map_error(FunctionT&& mapper) && noexcept {
        return result_detail::map_error_impl(poly::forward<FunctionT>(mapper),poly::move(storage_));
    }
    template <class FunctionT>
    NODISCARD auto map_error(FunctionT&& mapper) const&& noexcept {
        return result_detail::map_error_impl(poly::forward<FunctionT>(mapper),poly::move(storage_));
    }

    /**
     * @brief Applies either a mapper, or a fallback function depending on the contained value.
     * @param mapper A callable mapping `T -> U`
     * @param fallback A callable mapping `E -> U`
     * @returns The value returned either from mapper or fallback.
     *
     * Maps a `result<T, E>` to U by applying a mapper on a contained ok value, or a fallback
     * on a contained error.
     */
    template <class FunctionT, class FallbackT>
    NODISCARD auto map_or_else(FunctionT&& mapper, FallbackT&& fallback) & noexcept {
        return result_detail::map_or_else_impl(
            poly::forward<FunctionT>(mapper), poly::forward<FallbackT>(fallback), storage_);
    }
    template <class FunctionT, class FallbackT>
    NODISCARD auto map_or_else(FunctionT&& mapper, FallbackT&& fallback) const& noexcept {
        return result_detail::map_or_else_impl(
            poly::forward<FunctionT>(mapper), poly::forward<FallbackT>(fallback), storage_);
    }
    template <class FunctionT, class FallbackT>
    NODISCARD auto map_or_else(FunctionT&& mapper, FallbackT&& fallback) && noexcept {
        return result_detail::map_or_else_impl(poly::forward<FunctionT>(mapper),
                                               poly::forward<FallbackT>(fallback),
                                              poly::move(storage_));
    }
    template <class FunctionT, class FallbackT>
    NODISCARD auto map_or_else(FunctionT&& mapper, FallbackT&& fallback) const&& noexcept {
        return result_detail::map_or_else_impl(poly::forward<FunctionT>(mapper),
                                               poly::forward<FallbackT>(fallback),
                                              poly::move(storage_));
    }

    /**
     * @brief Calls func if the result is ok, otherwise it returns the error value
     * @param func A callable mapping `T -> result<U,E>`
     * @returns A `result<U, E>`
     *
     * For convenience `func` may also return an ok or error value. The return type of `and_then`
     * in these different cases are
     *
     * | *func* return type    | *result<T, E>::and_then* return type   |
     * |-----------------------|----------------------------------------|
     * | **result<U, E>**      | **result<U, E>**                       |
     * | **ok(U)**             | **result<U, E>**                       |
     * | **error(E)**          | **result<T, E>**                       |
     * | *error(F)*            | *Invalid*                              |
     *
     */
    template <class FunctionT>
    NODISCARD auto and_then(FunctionT&& func) & noexcept {
        return result_detail::and_then_impl(poly::forward<FunctionT>(func), storage_);
    }
    template <class FunctionT>
    NODISCARD auto and_then(FunctionT&& func) const& noexcept {
        return result_detail::and_then_impl(poly::forward<FunctionT>(func), storage_);
    }
    template <class FunctionT>
    NODISCARD auto and_then(FunctionT&& func) && noexcept {
        return result_detail::and_then_impl(poly::forward<FunctionT>(func),poly::move(storage_));
    }
    template <class FunctionT>
    NODISCARD auto and_then(FunctionT&& func) const&& noexcept {
        return result_detail::and_then_impl(poly::forward<FunctionT>(func),poly::move(storage_));
    }

    /**
     * @brief Call `func` if the result is an error, otherwise returns the contained ok.
     * @param func A callable mapping `E -> result<T, F>`
     * @returns A `result<T, F>`
     *
     * For convenience `func` may also return an ok or error value. The return type of `or_else`
     * in these different cases are
     *
     * | *func* return type    | *result<T, E>::or_else* return type   |
     * |-----------------------|----------------------------------------|
     * | **result<T, F>**      | **result<T, F>**                       |
     * | **ok(T)**             | **result<T, E>**                       |
     * | **error(F)**          | **result<T, F>**                       |
     * | *ok(U)*               | *Invalid*                              |
     *
     */
    template <class FunctionT>
    NODISCARD auto or_else(FunctionT&& func) & noexcept {
        return result_detail::or_else_impl(poly::forward<FunctionT>(func), storage_);
    }
    template <class FunctionT>
    NODISCARD auto or_else(FunctionT&& func) const& noexcept {
        return result_detail::or_else_impl(poly::forward<FunctionT>(func), storage_);
    }
    template <class FunctionT>
    NODISCARD auto or_else(FunctionT&& func) && noexcept {
        return result_detail::or_else_impl(poly::forward<FunctionT>(func),poly::move(storage_));
    }
    template <class FunctionT>
    NODISCARD auto or_else(FunctionT&& func) const&& noexcept {
        return result_detail::or_else_impl(poly::forward<FunctionT>(func),poly::move(storage_));
    }

    /**
     * @brief Unwraps the contained ok value.
     * @returns The contained contained ok value.
     *
     * @warning Calls `poly::panic` if `is_ok() == false`
     */
    template <class U = T,
        enable_if_t<!is_void<U>::value && is_same<U, T>::value>* = nullptr>
    NODISCARD U& unwrap() & noexcept {
        if (!storage_.has_val()) {
            poly::panic();
        }
        return storage_.val_.value;
    }

    template <class U = T,
        enable_if_t<!is_void<U>::value && is_same<U, T>::value>* = nullptr>
    NODISCARD const U& unwrap() const& noexcept {
        if (!storage_.has_val()) {
            poly::panic();
        }
        return storage_.val_.value;
    }

    template <class U = T,
        enable_if_t<!is_void<U>::value && is_same<U, T>::value>* = nullptr>
    NODISCARD U&& unwrap() && noexcept {
        if (!storage_.has_val()) {
            poly::panic();
        }
        return poly::move(storage_.val_.value);
    }

    template <class U = T,
        enable_if_t<!is_void<U>::value && is_same<U, T>::value>* = nullptr>
    NODISCARD const U&& unwrap() const&& noexcept {
        if (!storage_.has_val()) {
            poly::panic();
        }
        return poly::move(storage_.val_.value);
    }

    /**
     * @brief Unwraps a void ok value
     * @returns Nothing
     *
     * @warning Calls `poly::panic` if `is_ok() == false`
     */
    template <class U = T,
        enable_if_t<is_void<U>::value && is_same<U, T>::value>* = nullptr>
    U unwrap() & noexcept {
        if (!is_ok()) {
            poly::panic();
        }
    }

    template <class U = T,
        enable_if_t<is_void<U>::value && is_same<U, T>::value>* = nullptr>
    U unwrap() const& noexcept {
        if (!is_ok()) {
            poly::panic();
        }
    }

    template <class U = T,
        enable_if_t<is_void<U>::value && is_same<U, T>::value>* = nullptr>
    U unwrap() && noexcept {
        if (!is_ok()) {
            poly::panic();
        }
    }

    template <class U = T,
        enable_if_t<is_void<U>::value && is_same<U, T>::value>* = nullptr>
    U unwrap() const&& noexcept {
        if (!is_ok()) {
            poly::panic();
        }
    }

    /**
     * @brief Unwraps a result, yielding the contents of an ok. If the value it returns `backup`
     * @param backup A backup value that will be returned if the result is an error.
     * @returns Either the contained ok value, or the backup value.
     *
     * The backup value must be convertible to T. This function is not available for `result<void,
     * E>`.
     */
    template <
        class U,
        enable_if_t<!is_void<T>::value && is_convertible<U, T>::value>* = nullptr>
    NODISCARD auto unwrap_or(U&& backup) & noexcept {
        return result_detail::unwrap_or_impl(poly::forward<U>(backup), storage_);
    }
    template <
        class U,
        enable_if_t<!is_void<T>::value && is_convertible<U, T>::value>* = nullptr>
    NODISCARD auto unwrap_or(U&& backup) const& noexcept {
        return result_detail::unwrap_or_impl(poly::forward<U>(backup), storage_);
    }
    template <
        class U,
        enable_if_t<!is_void<T>::value && is_convertible<U, T>::value>* = nullptr>
    NODISCARD auto unwrap_or(U&& backup) && noexcept {
        return result_detail::unwrap_or_impl(poly::forward<U>(backup),poly::move(storage_));
    }
    template <
        class U,
        enable_if_t<!is_void<T>::value && is_convertible<U, T>::value>* = nullptr>
    NODISCARD auto unwrap_or(U&& backup) const&& noexcept {
        return result_detail::unwrap_or_impl(poly::forward<U>(backup),poly::move(storage_));
    }

    /**
     * @brief Unwraps a result, yielding the contents of an ok. If the value is an error it calls
     * fallback with its value
     * @param fallback A fallback mapping `E -> T`
     * @returns Either the contained ok value, or the result of `fallback(E)`
     */
    template <class FunctionT>
    NODISCARD auto unwrap_or_else(FunctionT&& fallback) & noexcept {
        return result_detail::unwrap_or_else_impl(poly::forward<FunctionT>(fallback), storage_);
    }
    template <class FunctionT>
    NODISCARD auto unwrap_or_else(FunctionT&& fallback) const& noexcept {
        return result_detail::unwrap_or_else_impl(poly::forward<FunctionT>(fallback), storage_);
    }
    template <class FunctionT>
    NODISCARD auto unwrap_or_else(FunctionT&& fallback) && noexcept {
        return result_detail::unwrap_or_else_impl(poly::forward<FunctionT>(fallback),
                                                 poly::move(storage_));
    }
    template <class FunctionT>
    NODISCARD auto unwrap_or_else(FunctionT&& fallback) const&& noexcept {
        return result_detail::unwrap_or_else_impl(poly::forward<FunctionT>(fallback),
                                                 poly::move(storage_));
    }

    /**
     * @brief Unwraps the contained error.
     * @returns The contained contained error
     *
     * @warning Calls `poly::panic` if `is_ok() == true`
     */
    NODISCARD E& unwrap_error() & noexcept {
        if (storage_.has_val()) {
            poly::panic();
        }
        return storage_.err_.value;
    }

    NODISCARD const E& unwrap_error() const& noexcept {
        if (storage_.has_val()) {
            poly::panic();
        }
        return storage_.err_.value;
    }

    NODISCARD E&& unwrap_error() && noexcept {
        if (storage_.has_val()) {
            poly::panic();
        }
        return poly::move(storage_.err_.value);
    }

    NODISCARD const E&& unwrap_error() const&& noexcept {
        if (storage_.has_val()) {
            poly::panic();
        }
        return poly::move(storage_.err_.value);
    }

    constexpr explicit operator bool() const noexcept {
        return is_ok();
    }

    //template<class V, class U=T, poly::enable_if_t<!poly::is_void_v<U> && poly::is_void_v<V>>* = nullptr>
    template<class U=T, poly::enable_if_t<!poly::is_void_v<U>>* = nullptr>
    operator result<void, E>() & {
        if(is_ok())
        {
            return poly::ok();
        }
        return poly::error(storage_.err_.value);
    }

    template<class V, class U=T, poly::enable_if_t<!poly::is_void_v<U> && poly::is_void_v<V>>* = nullptr>
    operator result<V, E>() const& {
        if(is_ok())
        {
            return poly::ok();
        }
        return poly::error(storage_.err_.value);
    }

    template<class V, class U=T, poly::enable_if_t<!poly::is_void_v<U> && poly::is_void_v<V>>* = nullptr>
    operator result<V, E>() && {
        if(is_ok())
        {
            return poly::ok();
        }
        return poly::error(move(storage_.err_.value));
    }

    template<class V, class U=T, poly::enable_if_t<!poly::is_void_v<U> && poly::is_void_v<V>>* = nullptr>
    operator result<V, E>() const && {
        if(is_ok())
        {
            return poly::ok();
        }
        return poly::error(move(storage_.err_.value));
    }

    /**
     * @brief Shorthand to ignore any stored value and simply map to value to void.
     * @return An ok void result if result contains an ok value, otherwise the contained error.
     */
    NODISCARD result<void, E> ignore_value() & noexcept
    {
        if(storage_.has_val())
        {
            return ok();
        }
        return error(storage_.err_.value);
    }

    NODISCARD result<void, E> ignore_value() const & noexcept
    {
        if(storage_.has_val())
        {
            return ok();
        }
        return error(storage_.err_.value);
    }

    NODISCARD result<void, E> ignore_value() && noexcept
    {
        if(storage_.has_val())
        {
            return ok();
        }
        return error(move(storage_.err_.value));
    }

    NODISCARD result<void, E> ignore_value() const && noexcept
    {
        if(storage_.has_val())
        {
            return ok();
        }
        return error(move(storage_.err_.value));
    }

private:
    template <class U, class F>
    friend class result;

    friend struct result_detail::result_comparator;

    result_detail::result_storage<result_detail::ok_tag<T>, result_detail::error_tag<E>> storage_;
};

template <class T, class E, class U, class F>
bool operator==(const result<T, E>& lhs, const result<U, F>& rhs) noexcept(
noexcept(result_detail::result_comparator::equal(lhs, rhs))) {
    return result_detail::result_comparator::equal(lhs, rhs);
}

template <class T, class E, class U, class F>
constexpr bool operator!=(const result<T, E>& lhs,
                          const result<U, F>& rhs) noexcept(noexcept(lhs == rhs)) {
    return !(lhs == rhs);
}

} // namespace poly

#undef LAUNDER
#undef NODISCARD