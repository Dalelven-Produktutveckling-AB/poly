#pragma once

#include <string.h>

#include <cstddef>
#include <new>

#include "type_traits.hpp"
#include "utility.hpp"

namespace poly
{
/**
 * @brief A `poly::function` equivalent with in-place storage
 * @tparam T The function signature on the same format as `poly::function`
 * @tparam Capacity The storage capacity
 *
 * The capacity is statically checked to make sure that all callable objects will fit.
 *
 * It is legal to assign a basic function with a smaller capacity to one with a larger capacity
 * but not the opposite.
 *
 * This makes it possible to store capturing lambdas as well as function pointers etc.
 */
template<class T, size_t Capacity>
class basic_function;

namespace detail
{
template<class R, class...Args>
struct function_vtable
{
    R (*invoke)(void*, Args...);
    void (*copy)(const void*, void*);
    void (*move)(void*, void*);
    void (*destroy)(void*);

    template<class T>
    static R invoke_for(void* ptr, Args...args) {
        return (*static_cast<T*>(ptr))(poly::forward<Args>(args)...);
    }

    template<class T>
    static void copy_for(const void* src, void *dst)
    {
        if constexpr(is_trivially_copyable_v<T>)
        {
            memcpy(dst, src, sizeof(T));
        }
        else
        {
            ::new(dst) T(*static_cast<const T*>(src));
        }
    }

    template<class T>
    static void move_for(void* src, void *dst)
    {
        if constexpr(is_trivially_copyable_v<T>)
        {
            memcpy(dst, src, sizeof(T));
        }
        else
        {
            ::new(dst) T(poly::move(*static_cast<T *>(src)));
        }
    }

    template<class T>
    static void destroy_for(void *src)
    {
        static_cast<T*>(src)->~T();
    }

#ifdef POLY_TEST_INSTRUMENTATION
    size_t type_size = 0;
#endif

    template<class T>
    static function_vtable* vtable_for() {
        static function_vtable table {
            invoke_for<T>,
            copy_for<T>,
            move_for<T>,
            destroy_for<T>
#ifdef POLY_TEST_INSTRUMENTATION
            ,sizeof(T)
#endif
        };

        return &table;
    }
};

template<size_t Cap, bool GE8 = (Cap >= 8), bool GE4 = (Cap >= 4)>
struct function_buffer_alignment
{
    static constexpr std::size_t value = alignof(uint64_t) >= alignof(void(*)()) ?
                                         alignof(uint64_t) : alignof(void(*)());
};

template<size_t Cap>
struct function_buffer_alignment<Cap, false, true>
{
    static constexpr std::size_t value = 4;
};

template<size_t Cap>
struct function_buffer_alignment<Cap, false, false>
{
    static constexpr std::size_t value = 1;
};

template<size_t Capacity>
static constexpr std::size_t function_buffer_alignment_v = function_buffer_alignment<Capacity>::value;

template<class T>
struct is_basic_function: poly::false_type {};

template<class R, class...Args, std::size_t C>
struct is_basic_function<basic_function<R(Args...), C>>: poly::true_type {};

template<class T>
inline constexpr bool is_basic_function_v = is_basic_function<T>::value;

}

template<class R, class...Args, size_t Capacity>
class basic_function<R(Args...), Capacity>
{
    alignas(detail::function_buffer_alignment_v<Capacity>) unsigned char buffer_[Capacity];
    detail::function_vtable<R, Args...> *vtable_;

    template<class, size_t>
    friend class basic_function;

    template<class O>
    static void static_check() {
        static_assert(sizeof(O) <= Capacity, "Function object too large to fit in buffer");
        static_assert(alignment_of_v<O> <= detail::function_buffer_alignment_v<Capacity>,
                      "Object alignment not compatible with buffer alignment");
        static_assert(is_convertible_v<decltype(declval<O>()(declval<Args>()...)), R>);
        static_assert(is_copy_constructible_v<O>, "Function can only store function objects that are copy constructible");
    }

    void destroy_me() {
        vtable_->destroy(buffer_);
    }
public:
    template<class R2 = R, poly::enable_if_t<poly::is_same_v<R2, void>>* = nullptr>
    basic_function(): basic_function(+[](Args...) {}) {}

    template<class Object,
            enable_if_t<!detail::is_basic_function_v<decay_t<Object>>>* = nullptr>
    basic_function(Object&& object) {
        if constexpr(poly::is_convertible_v<Object, R(*)(Args...)>)
        {
            using pointer_t = R(*)(Args...);
            static_check<pointer_t>();
            ::new(static_cast<void *>(buffer_)) pointer_t(poly::forward<Object>(object));
            vtable_ = detail::function_vtable<R, Args...>::template vtable_for<pointer_t>();
        }
        else
        {
            using object_t = decay_t<Object>;
            static_check<object_t>();

            ::new(static_cast<void *>(buffer_)) object_t(poly::forward<Object>(object));
            vtable_ = detail::function_vtable<R, Args...>::template vtable_for<object_t>();
        }
    }

    template<size_t Cap>
    basic_function(const basic_function<R(Args...), Cap> &rhs)
    {
        static_assert(Cap <= Capacity, "Cannot assign larger basic_function to smaller");
        rhs.vtable_->copy(rhs.buffer_, buffer_);
        vtable_ = rhs.vtable_;
    }

    template<size_t Cap>
    basic_function(basic_function<R(Args...), Cap> &&rhs)
    {
        static_assert(Cap <= Capacity, "Cannot assign larger basic_function to smaller");
        rhs.vtable_->move(rhs.buffer_, buffer_);
        vtable_ = rhs.vtable_;
    }

    basic_function(const basic_function& rhs) {
        rhs.vtable_->copy(rhs.buffer_, buffer_);
        vtable_ = rhs.vtable_;
    }
    basic_function(basic_function&& rhs)  noexcept {
        rhs.vtable_->move(rhs.buffer_, buffer_);
        vtable_ = rhs.vtable_;
    }
    ~basic_function() {
        destroy_me();
    }

    basic_function& operator=(const basic_function& rhs) {
        if(&rhs == this) {
            return *this;
        }
        destroy_me();
        vtable_ = rhs.vtable_;
        vtable_->copy(rhs.buffer_, buffer_);
        return *this;
    }
    basic_function& operator=(basic_function&& rhs) {
        if(&rhs == this) {
            return *this;
        }
        destroy_me();
        vtable_ = rhs.vtable_;
        vtable_->move(rhs.buffer_, buffer_);
        return *this;
    }

    template<size_t Cap2>
    basic_function& operator=(const basic_function<R(Args...), Cap2> &rhs)
    {
        static_assert(Capacity > Cap2, "Cannot assign larger basic_function to smaller");
        destroy_me();
        vtable_ = rhs.vtable_;
        vtable_->copy(rhs.buffer_);
        return *this;
    }

    template<size_t Cap2>
    basic_function& operator=(basic_function<R(Args...), Cap2> &&rhs)
    {
        static_assert(Capacity > Cap2, "Cannot assign larger basic_function to smaller");
        destroy_me();
        vtable_ = rhs.vtable_;
        vtable_->move(rhs.buffer_, buffer_);
        return *this;
    }

    template<class Object, enable_if_t<!detail::is_basic_function_v<decay_t<Object>>>* = nullptr>
    basic_function& operator=(Object&& object)
    {
        destroy_me();
        if constexpr(poly::is_convertible_v<Object, R(*)(Args...)>)
        {
            using pointer_t = R(*)(Args...);
            static_check<pointer_t>();
            ::new(static_cast<void *>(buffer_)) pointer_t(poly::forward<Object>(object));
            vtable_ = detail::function_vtable<R, Args...>::template vtable_for<pointer_t>();
        }
        else
        {
            using object_t = decay_t<Object>;
            static_check<object_t>();
            ::new(static_cast<void *>(buffer_)) object_t(poly::forward<Object>(object));
            vtable_ = detail::function_vtable<R, Args...>::template vtable_for<object_t>();
        }
        return *this;
    }

    R operator()(Args...args) {
        return vtable_->invoke(static_cast<void*>(buffer_), poly::forward<Args>(args)...);
    }

#ifdef POLY_TEST_INSTRUMENTATION
    size_t size() const
    {
        return vtable_->type_size;
    }
#endif
};

/**
 * @brief A user-friendly alias for a `function` with a "good" default capacity.
 */
template<class T>
using function = basic_function<T, 4*sizeof(void*)>;

namespace invoke_detail {
    template <class>
    constexpr bool is_reference_wrapper_v = false;

    template <class T, class Type, class T1, class... Args>
    constexpr decltype(auto) INVOKE(Type T::* f, T1&& t1, Args&&... args)
    {
        if constexpr (poly::is_member_function_pointer_v<decltype(f)>) {
            if constexpr (poly::is_base_of_v<T, poly::decay_t<T1>>)
                return (poly::forward<T1>(t1).*f)(poly::forward<Args>(args)...);
            else if constexpr (is_reference_wrapper_v<poly::decay_t<T1>>)
                return (t1.get().*f)(poly::forward<Args>(args)...);
            else
                return ((*poly::forward<T1>(t1)).*f)(poly::forward<Args>(args)...);
        } else {
            static_assert(poly::is_member_object_pointer_v<decltype(f)>);
            static_assert(sizeof...(args) == 0);
            if constexpr (poly::is_base_of_v<T, poly::decay_t<T1>>)
                return poly::forward<T1>(t1).*f;
            else if constexpr (is_reference_wrapper_v<poly::decay_t<T1>>)
                return t1.get().*f;
            else
                return (*poly::forward<T1>(t1)).*f;
        }
    }

    template <class F, class... Args>
    constexpr decltype(auto) INVOKE(F&& f, Args&&... args)
    {
        return poly::forward<F>(f)(poly::forward<Args>(args)...);
    }
} // namespace detail

template< class F, class... Args>
constexpr poly::invoke_result_t<F, Args...> invoke(F&& f, Args&&... args)
{
    return invoke_detail::INVOKE(poly::forward<F>(f), poly::forward<Args>(args)...);
}

namespace detail
{
template<class T>
struct mem_fn_impl
{
    T fn_;
    template<class...Args>
    auto operator()(Args&&...args) -> poly::invoke_result_t<T, Args&&...>
    {
        return poly::invoke(fn_, poly::forward<Args>(args)...);
    }
};
}

template<class M, class T>
auto mem_fn(M T::* pm)
{
    return detail::mem_fn_impl<decltype(pm)>{pm};
}

}