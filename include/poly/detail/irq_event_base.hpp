#pragma once

namespace poly
{
class irq_event_runtime;
namespace detail
{
class irq_event_base
{
    friend class poly::irq_event_runtime;
    // next_ pointer is used by the runtime to
    // form a linked list of events that are ready to
    // run.
    irq_event_base* next_ = nullptr;
public:
    virtual ~irq_event_base() = default;
    virtual void run_callback() = 0;
};
}
}