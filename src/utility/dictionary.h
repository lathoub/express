#pragma once

#include "vector.h"

template <class T, class U>
class dictionary_item
{
public:
    T key;
    U value;
};

template <class T, class U, size_t MaxSize>
class dictionary_iterator
{
    typedef dictionary_item<T, U> dictionary_item;
    typedef vector<dictionary_item, MaxSize> dictionary_items;
    typedef dictionary_iterator<T, U, MaxSize> iterator;
    typedef dictionary_iterator<T, U, MaxSize> const_iterator;

public:
    explicit dictionary_iterator(dictionary_items* kvps) : values_ptr_{ kvps }, position_{ 0 }
    {
    }

    dictionary_iterator(dictionary_items* kvps, const size_t size) : values_ptr_{ kvps }, position_{ size }
    {
    }

    bool operator!=(const_iterator& other) const
    {
        return !(*this == other);
    }

    bool operator==(const_iterator& other) const
    {
        return position_ == other.position_;
    }

    dictionary_iterator& operator++()
    {
        ++position_;
        return *this;
    }

    dictionary_item& operator*() const
    {
        return values_ptr_->at(position_);
    }

private:
    dictionary_items* values_ptr_;

    size_t position_;
};

template <class T, class U, size_t MaxSize = 10>
class dictionary
{
    typedef dictionary_item<T, U> dictionary_item;
    typedef vector<dictionary_item, MaxSize> dictionary_items;

    dictionary_items kvps_{};

    auto get(T key) -> U&
    {
        for (size_t i = 0; i < kvps_.size(); i++)
        {
            if (kvps_.at(i).key == key)
            {
                return kvps_.at(i).value;
            }
        }

        dictionary_item item{};
        item.key = key;
        item.value = U{};

        kvps_.push_back(item);

        return kvps_.back().value;
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
        return kvps_.size();
    }

public:

    typedef dictionary_iterator<T,U, MaxSize> iterator;

    iterator begin()
    {
        return iterator(&kvps_);
    }

    iterator end()
    {
        return iterator(&kvps_, kvps_.size());
    }

    typedef dictionary_iterator<T,U, MaxSize> const_iterator;

    const_iterator begin() const
    {
        return const_iterator(&kvps_);
    }

    const_iterator end() const
    {
        return const_iterator(&kvps_, kvps_.size());
    }

};
