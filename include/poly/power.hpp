#pragma once

#include "platform/power.hpp"

namespace poly::power
{
using namespace poly::platform::power;

/**
 * @brief A class to hold a request for a power mode
 *
 * The request will automatically handle downgrading or upgrading when it is reassigned,
 * and releasing itself when it is destroyed.
 */
class power_request
{
public:
    power_request(const power_request& rhs);
    power_request& operator=(const power_request& rhs);
    ~power_request();

    power_mode current_mode() {
        return current_mode_;
    }
private:
    power_mode current_mode_;

    explicit power_request(power_mode mode) noexcept: current_mode_(mode) {}
    friend power_request request_minimum_power_mode(power_mode mode);
};

/**
 * @brief Request a minimum power mode
 * @param mode The minimum power mode to request
 * @return A `power_requst` object that must be kept alive for as long as the request is valid.
 */
power_request request_minimum_power_mode(power_mode mode);
/**
 * @brief Get the requested power mode.
 * @return The highest power mode that is currently requested.
 *
 * If no specific power modeis requested then `poly::power::default_mode` is returned.
 */
power_mode requested_power_mode();
}