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