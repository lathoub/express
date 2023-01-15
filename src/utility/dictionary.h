#pragma once

#include "vector.h"

template <class T, class U, size_t max_size_>
class dictionary
{
    vector<T, max_size_> keys_{};
    vector<U, max_size_> values_{};

    auto get(T key) -> U&
    {
        for (size_t i = 0; i < keys_.size(); i++)
        {
            if (keys_.at(i) == key)
            {
                return values_.at(i);
            }
        }

        // not found, so add it with empty value
        keys_.push_back(key);
        values_.push_back(U{});

        return values_.back();
    }

public:
    const U& operator[](T key) const
    {
        return get(key);
    }

    U& operator[](T key)
    {
        return get(key);
    }

    auto length() -> size_t
    {
        return keys_.size();
    }
};
