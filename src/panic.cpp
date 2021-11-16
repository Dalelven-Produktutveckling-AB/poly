#include <poly/panic.hpp>
#include <poly/config.hpp>

#ifdef POLY_CONFIG_PANIC_STD_TERMINATE
#include <exception>
#endif

static void (*panic_handler)() = nullptr;

namespace poly
{
void panic()
{
    if(panic_handler) {
        panic_handler();
    }
#ifdef POLY_CONFIG_PANIC_STD_TERMINATE
    std::terminate();
#endif
    while(true) {}
}
void set_panic_handler(void (*handler)())
{
    panic_handler = handler;
}
}