#pragma once

namespace poly::platform::power
{
enum class power_mode
{
    mode1,
    mode2,
    mode3,
    mode4,
    end,
};
inline constexpr power_mode default_mode = power_mode::mode2;
}