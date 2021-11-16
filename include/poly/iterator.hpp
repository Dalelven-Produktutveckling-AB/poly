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