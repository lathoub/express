#pragma once

#include "vector.h"

/// @brief
/// @tparam T
/// @tparam U
template <class T, class U>
class dictionary_item
{
public:
    T key;
    U value;
};

/// @brief
/// @tparam T
/// @tparam U
/// @tparam MaxSize
template <class T, class U, size_t MaxSize>
class dictionary_iterator
{
    typedef dictionary_item<T, U> dictionary_item_;
    typedef vector<dictionary_item_, MaxSize> dictionary_items;
    typedef dictionary_iterator<T, U, MaxSize> iterator;
    typedef dictionary_iterator<T, U, MaxSize> const_iterator;

public:
    /// @brief
    /// @param kvps
    explicit dictionary_iterator(dictionary_items *kvps) : values_ptr_{kvps}, position_{0}
    {
    }

    /// @brief
    /// @param kvps
    /// @param size
    dictionary_iterator(dictionary_items *kvps, const size_t size) : values_ptr_{kvps}, position_{size}
    {
    }

    /// @brief
    /// @param other
    /// @return
    bool operator!=(const_iterator &other) const
    {
        return !(*this == other);
    }

    /// @brief
    /// @param other
    /// @return
    bool operator==(const_iterator &other) const
    {
        return position_ == other.position_;
    }

    /// @brief
    /// @return
    dictionary_iterator &operator++()
    {
        ++position_;
        return *this;
    }

    /// @brief
    /// @return
    dictionary_item_ &operator*() const
    {
        return values_ptr_->at(position_);
    }

private:
    dictionary_items *values_ptr_;

    size_t position_;
};

/// @brief
/// @tparam T
/// @tparam U
/// @tparam MaxSize
template <class T, class U, size_t MaxSize = 10>
class dictionary
{
    typedef dictionary_item<T, U> dictionary_item_;
    typedef vector<dictionary_item_, MaxSize> dictionary_items;

    dictionary_items kvps_{};

    auto get(T key) -> U &
    {
        for (size_t i = 0; i < kvps_.size(); i++)
            if (kvps_.at(i).key == key)
                return kvps_.at(i).value;

        dictionary_item_ item{};
        item.key = key;
        item.value = U{};

        kvps_.push_back(item);

        return kvps_.back().value;
    }

public:
    const U &operator[](T key) const
    {
        return get(key);
    }

    U &operator[](T key)
    {
        return get(key);
    }

    /// @brief 
    /// @return 
    auto length() -> size_t
    {
        return kvps_.size();
    }

    /// @brief Count elements with a specific key. Searches the container for elements 
    /// with a key equivalent to k and returns the number of matches.
    /// @param key 
    /// @return 
    auto count(T key) -> size_t
    {
        size_t n = 0;
        for (size_t i = 0; i < kvps_.size(); i++)
            if (kvps_.at(i).key == key)
                n++;
        return n;
    }

    auto clear() -> void
    {
        kvps_.clear();
    }

public:
    typedef dictionary_iterator<T, U, MaxSize> iterator;

    iterator begin()
    {
        return iterator(&kvps_);
    }

    iterator end()
    {
        return iterator(&kvps_, kvps_.size());
    }

    typedef dictionary_iterator<T, U, MaxSize> const_iterator;

    const_iterator begin() const
    {
        return const_iterator(&kvps_);
    }

    const_iterator end() const
    {
        return const_iterator(&kvps_, kvps_.size());
    }
};
