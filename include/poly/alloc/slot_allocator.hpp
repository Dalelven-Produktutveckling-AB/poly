/**
* Copyright © 2021 Dalelven Produktutveckling AB
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
* IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <etl/array.h>
#include <etl/queue_spsc_atomic.h>

#include <poly/config.hpp>

namespace poly::alloc
{
/**
 * @brief An allocator for `Slots` number of items of type T
 * @tparam T The type to allocate
 * @tparam Slots The number of slots available using this allocator.
 *
 * `T` must be default-constructible to be usable.
 */
template<class T, size_t Slots>
class slot_allocator
{
    /**
     * @brief Backing storage of the slots
     */
    etl::array<T, Slots> slot_storage_;
    /**
     * @brief The queue of available slots.
     *
     * All items in this queue must always only point to items inside `slot_storage_`.
     */
    etl::queue_spsc_atomic<T*, Slots> available_slots_;
public:
    /**
     * @brief Default constructor. The entire storage is available at this point.
     */
    slot_allocator() {
        for(auto& slot: slot_storage_) {
            available_slots_.push(&slot);
        }
    }

    /**
     * @brief The allocator is not copyable or movable
     *
     * Anything is is prone to error. Typical usage is to statically allocate an instance of the allocator
     * and use that.
     */
    slot_allocator(const slot_allocator&) = delete;
    slot_allocator(slot_allocator&&) = delete;

    slot_allocator& operator=(const slot_allocator&) = delete;
    slot_allocator& operator=(slot_allocator&&) = delete;

    /**
     * @brief Try to allocate a `T`
     * @return `nullptr` if the allocation failed, otherwise a pointer to the allocated `T`.
     */
    T* try_allocate() {
        T* retval;
        if(!available_slots_.pop(retval))
        {
            return nullptr;
        }
        return retval;
    }

    /**
     * @brief Try to deallocate a pointer
     * @param ptr The pointer to deallocate
     * @return true if the deallocation was successful.
     *
     * If this function returns false then most likely some data corruption or a very bad bug is present.
     * It should be treated as an exception and most likely a fatal error that cannot be recovered from.
     */
    bool try_deallocate(T* ptr) {
        return available_slots_.push(ptr);
    }
};

#ifdef POLY_CONFIG_ENABLE_DYNAMIC_ALLOC
template<class T>
class slot_allocator<T, 0>
{
public:
    slot_allocator() = default;
    slot_allocator(const slot_allocator&) = delete;
    slot_allocator(slot_allocator&&) = delete;

    slot_allocator& operator=(const slot_allocator&) = delete;
    slot_allocator& operator=(slot_allocator&&) = delete;

    T* try_allocate() {
        return new T();
    }

    bool try_deallocate(T* ptr) {
        delete ptr;
        return true;
    }
};
#endif
}