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

#include <poly/power.hpp>
#include <etl/array.h>

static etl::array<int, static_cast<size_t>(poly::power::power_mode::end)> power_requests_{};

namespace poly::power
{
power_request::power_request(const power_request &rhs) {
    current_mode_ = rhs.current_mode_;
    power_requests_[static_cast<size_t>(current_mode_)]++;
}

power_request& power_request::operator=(const power_request& rhs) {
    if(current_mode_ == rhs.current_mode_) {
        return *this;
    }

    power_requests_[static_cast<size_t>(current_mode_)]--;
    current_mode_ = rhs.current_mode_;
    power_requests_[static_cast<size_t>(current_mode_)]++;
    return *this;
}

power_request::~power_request() {
    power_requests_[static_cast<size_t>(current_mode_)]--;
}

power_request request_minimum_power_mode(power_mode mode)
{
    power_requests_[static_cast<size_t>(mode)]++;
    return power_request(mode);
}
power_mode requested_power_mode()
{
    size_t i=0;
    for(; i<power_requests_.size(); i++) {
        if(power_requests_[i] > 0) {
            return static_cast<power_mode>(i);
        }
    }
    return default_mode;
}
}