#pragma once

namespace poly::platform::power
{
enum class power_mode
{
    hfclk_active,
    lfclk_active,
    end,
};
inline constexpr power_mode default_mode = power_mode::lfclk_active;
}