#pragma once

#ifndef POLY_PLATFORM_PC
#warning "POLY_PLATFORM_PC macro not set, PC platform is assumed"
#endif

namespace poly::platform::power
{
enum class power_mode
{
    running,
    end,
};
inline constexpr power_mode default_mode = power_mode::running;
}