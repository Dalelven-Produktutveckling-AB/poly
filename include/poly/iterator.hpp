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

#include "utility.hpp"

namespace poly
{
template<class Container>
struct back_insert_iterator
{
protected:
    Container *container_;
public:
    using value_type = void;
    using distance = void;
    using pointer = void;
    using reference = void;

    explicit back_insert_iterator(Container& c): container_(&c) {}

    back_insert_iterator& operator=( const typename Container::value_type& value )
    {
        container_->push_back(value);
        return *this;
    }

    back_insert_iterator& operator=( typename Container::value_type&& value )
    {
        container_->push_back(poly::move(value));
        return *this;
    }

    back_insert_iterator& operator*() {
        return *this;
    }

    back_insert_iterator& operator++() {
        return *this;
    }
    back_insert_iterator operator++( int ) {
        return *this;
    }
};

template<class Container>
back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}
}