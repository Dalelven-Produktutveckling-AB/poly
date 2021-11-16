#pragma once

#if defined(POLY_PLATFORM_NRF52840)
#include "nrf52840/power.hpp"
#elif defined(POLY_PLATFORM_TESTING)
#include "testing/power.hpp"
#else
#include "pc/power.hpp"
#endif
