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

#include "irq_event.hpp"

#include "etl/intrusive_list.h"
#include "etl/intrusive_links.h"
#include "etl/optional.h"

#include "type_traits.hpp"

namespace poly
{
/**
 * @brief base class that must be used by all soft events.
 */
class soft_event_base: public etl::bidirectional_link<0>
{
public:
    soft_event_base() {
        this->clear();
    }

    /**
     * @brief Utility function to safely unlink this event from any list
     *
     */
    void safe_unlink()
    {
        this->unlink();
        this->clear();
    }
};

/**
 * @brief A list of soft event listeners.
 * @tparam EventT The event type for the listeners.
 *
 * The only operations that can be performed is to push, pop and steal listeners.
 */
template<class EventT>
class soft_event_list
{
    static_assert(poly::is_base_of_v<soft_event_base, EventT>, "Events must derive from soft_event_base");
    etl::intrusive_list<EventT> listeners_;
public:
    /**
     * @brief Pushes a listener to the back of the list of event listeners.
     * @param listener The event listener
     */
    void push(EventT& listener)
    {
        if(listener.is_linked())
        {
            listener.safe_unlink();
        }
        listeners_.push_back(listener);
    }

    /**
     * @brief Pops a listener from the front of the list of listeners
     * @return nullptr if there are no more event listeners, otherwise a pointer to the listener.
     */
    EventT* pop() {
        if(listeners_.empty()) {
            return nullptr;
        }
        EventT& evt = listeners_.front();
        listeners_.pop_front();
        evt.clear();
        return &evt;
    }

    /**
     * @brief Move all listeners from this list to `rhs`
     * @param rhs The list to move all listeners to
     */
    void move_to_front_of(soft_event_list& rhs) {
        rhs.listeners_.splice(rhs.listeners_.begin(), listeners_);
    }
};

/**
 * @brief Helper class to create soft event services
 * @tparam EventT The soft event type for this service
 *
 * A soft event service contains a list of active listeners and a list of
 * pending listeners. Pending listeners are moved from pending to active by
 * notifying the IRQ event which in turn should process all active listeners
 * and add any new listeners.
 *
 * See `poly/timer.cpp` for a usage example.
 */
template<class EventT>
class soft_event_service
{
    bool notifying_active_listeners_ = false;
    soft_event_list<EventT> active_listeners_;
    soft_event_list<EventT> pending_listeners_;
    irq_event<void> *irq_;
public:
    explicit soft_event_service(irq_event<void>& irq): irq_(&irq) {}
    /**
     * @brief Add a listener to the list of pending listeners
     * @param event The event listener to add
     *
     */
    void add_listener(EventT& event) {
        pending_listeners_.push(event);
        // If this is true we are already in a "post" context and should
        // not post again
        if(!notifying_active_listeners_)
        {
            irq_->post(irq_baton{});
        }
    }

    /**
     * @brief Notify listeners based on a predicate.
     * @tparam Predicate The predicate type
     * @param predicate The predicate to use when deciding which listener to notify.
     *
     * The predicate must have the signature `bool(EventT&)`. It must return true
     * if the listener should be notified. Otherwise it must return false.
     */
    template<class Predicate>
    void notify_active_listeners(Predicate&& predicate) {
        soft_event_list<EventT> local;
        active_listeners_.move_to_front_of(local);
        notifying_active_listeners_ = true;
        while(EventT* listener = local.pop())
        {
            if(predicate(*listener))
            {
                listener->notify();
            }
            else
            {
                active_listeners_.push(*listener);
            }
        }
        notifying_active_listeners_ = false;
    }

    /**
     * @brief Add pending listeners based on a predicate
     * @tparam Predicate The predicate type
     * @param predicate The predicate to use when deciding which listener to move from pending to active
     *
     * The predicate must have the signature `bool(const EventT&)`. It must return true
     * if the listener should move from pending to active. Otherwise it must return false.
     */
    template<class Predicate>
    void add_pending_listeners(Predicate&& predicate) {
        soft_event_list<EventT> local;
        pending_listeners_.move_to_front_of(local);

        while(EventT* listener = local.pop())
        {
            const EventT& cref = *listener;
            if(predicate(cref))
            {
                active_listeners_.push(*listener);
            }
            else
            {
                pending_listeners_.push(*listener);
            }
        }
    }
};
}