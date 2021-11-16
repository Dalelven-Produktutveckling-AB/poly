#pragma once

#include "type_traits.hpp"

namespace poly
{
/**
 * @brief Helper class to manually manage the lifetime of an object.
 * @tparam T The object to manage.
 *
 * This is mostly meant to be used to defer static initialization to runtime.
 *
 * This object is not copyable or movable at all.
 */
template<class T>
class manual_lifetime
{
    union {
        T data_;
    };
public:
    manual_lifetime() noexcept {} // NOLINT(modernize-use-equals-default)
    explicit manual_lifetime(const T& t) noexcept(poly::is_nothrow_constructible_v<T, const T&>) {
        emplace(t);
    }
    manual_lifetime(const manual_lifetime&) = delete;
    manual_lifetime(manual_lifetime&&) = delete;

    manual_lifetime& operator=(const manual_lifetime&) = delete;
    manual_lifetime& operator=(manual_lifetime&&) = delete;

    ~manual_lifetime() {} // NOLINT(modernize-use-equals-default)

    /**
     * @brief Destroy the contained object.
     *
     * @warning No checking if an actual object is constructed is done!
     */
    void destroy() {
        data_.~T();
    }

    /**
     * @brief Emplace a new object.
     * @tparam Args
     * @param args The arguments to construct with.
     * @return A reference to the contained object.
     *
     * @warning If an object is already present it will be overwritten without any destructor being called!
     */
    template<class...Args>
    poly::enable_if_t<poly::is_constructible_v<T, Args&&...>, T&>
    emplace(Args&&...args) noexcept(poly::is_nothrow_constructible_v<T, Args&&...>)
    {
        return *::new(static_cast<void*>(&data_)) T(static_cast<Args&&>(args)...);
    }

    /**
     * @brief Dereference operator
     * @return A reference to the contained `T`.
     */
    T& operator*() {
        return data_;
    }
    const T& operator*() const {
        return data_;
    }
    T* operator->() {
        return &data_;
    }

    const T* operator->() const {
        return &data_;
    }
};
}