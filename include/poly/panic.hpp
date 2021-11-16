#pragma once

namespace poly
{
[[noreturn]] void panic();
void set_panic_handler(void (*handler)());
}