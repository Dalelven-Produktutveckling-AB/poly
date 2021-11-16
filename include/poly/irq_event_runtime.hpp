#pragma once

#include "detail/irq_event_base.hpp"

#include "etl/atomic.h"

#include "poly/result.hpp"
#include "string_literal.hpp"

#include <cstddef>

namespace poly
{
struct irq_baton {};

/**
 * @brief An event runtime to take post IRQ events for future handling to
 *
 * Events are added as a single-linked list where each event contains a pointer to the next
 * event. New events are always added to the front of the list, which means we effectively have
 * a FILO queue. The list is reversed before running the events, so even though events are stored
 * in First In Last Out order, they will be completed in First In First Out order.
 */
class irq_event_runtime
{
    mutable etl::atomic<detail::irq_event_base*> pending_events_{nullptr};

    /**
     * @brief Reverse a single-linked list by iterating over it building a new linked list
     * @param list_head The old list head to reverse from
     * @return The head of the reversed list
     */
    detail::irq_event_base* reverse_list(detail::irq_event_base* list_head)
    {
        // We form a new list by taking all elements from the supplied list
        // pushing them to the front of the "retval" list
        detail::irq_event_base* retval_head = nullptr;
        while(list_head != nullptr)
        {
            auto* rest_of_list = list_head->next_;
            list_head->next_ = retval_head;
            retval_head = list_head;
            list_head = rest_of_list;
        }

        return retval_head;
    }
public:
    void post(irq_baton, detail::irq_event_base& evt)
    {
        // We want to add evt to the front of the linked list.
        // so evt.next_ needs to point to the current head of the list.
        evt.next_ = pending_events_.load(etl::memory_order_acquire);
        // Try to update the list head to evt, this will only succeed
        // if evt.next_ is the same as what is currently stored in pending_events_.
        while(!pending_events_.compare_exchange_strong(evt.next_, &evt))
        {
            // The compare exchange failed, compare_exchange_strong will update evt.next_ to contain
            // the current value stored in pending_events_.
        }
    }

    /**
     * @brief Runs all available events.
     *
     * This function returns when there are no more pending events to process.
     */
    void run_available() {
        while(true)
        {
            // "Take" the entire list of posted events and set the list to empty state
            // This list is in FILO (First In Last Out) order
            detail::irq_event_base *head = pending_events_.exchange(nullptr);
            if(head == nullptr)
            {
                // List was empty, do nothing
                break;
            }

            // Reverse the list to ensure we get the events in FIFO order
            head = reverse_list(head);

            while (head != nullptr) {
                auto* current = head;
                head = head->next_;
                current->run_callback();
            }
        }
    }

    /**
     * @brief Checks if any events are pending.
     * @return True if events are available.
     */
    [[nodiscard]] bool events_available() const {
        return pending_events_.load() != nullptr;
    }
};
}
