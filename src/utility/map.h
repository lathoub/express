#pragma once

#include "vector.h"

namespace std {

/// @brief
/// @tparam T
/// @tparam U
template <class T, class U>
class map_item
{
public:
    T first;
    U second;
};

/// @brief
/// @tparam T
/// @tparam U
/// @tparam MaxSize
template <class T, class U, size_t MaxSize>
class map_iterator
{
    typedef map_item<T, U> dictionary_item_;
    typedef vector<dictionary_item_, MaxSize> map_items;
    typedef map_iterator<T, U, MaxSize> iterator;

public:
    /// @brief
    /// @param kvps
    explicit map_iterator(map_items *kvps)
        : values_ptr_{kvps}, position_{0}
    {
    }

    /// @brief
    /// @param kvps
    explicit map_iterator(const map_items *kvps)
        : values_ptr_{kvps}, position_{0}
    {
    }

    /// @brief
    /// @param kvps
    /// @param size
    map_iterator(map_items *kvps, const size_t size)
        : values_ptr_{kvps}, position_{size}
    {
    }

    /// @brief
    /// @param kvps
    /// @param size
    map_iterator(const map_items *kvps, const size_t size)
        : values_ptr_{kvps}, position_{size}
    {
    }

    /// @brief
    /// @param other
    /// @return
    bool operator!=(const iterator &other) const
    {
        return !(*this == other);
    }

    /// @brief
    /// @param other
    /// @return
    bool operator==(const iterator &other) const
    {
        return position_ == other.position_;
    }

    /// @brief
    /// @return
    map_iterator &operator++()
    {
        ++position_;
        return *this;
    }

    /// @brief
    /// @return
    const map_item<T, U> &operator*() const
    {
        return values_ptr_->at(position_);
    }

private:
    const vector<map_item<T, U>, MaxSize> *values_ptr_;

    size_t position_;
};

/// @brief
/// @tparam T
/// @tparam U
/// @tparam MaxSize
template <class T, class U, size_t MaxSize = 10>
class map
{
    typedef map_item<T, U> dictionary_item_;
    typedef vector<dictionary_item_, MaxSize> map_items;

    map_items kvps_{};

    auto get(T first) -> U &
    {
        for (size_t i = 0; i < kvps_.size(); i++)
            if (kvps_.at(i).first == first)
                return kvps_.at(i).second;

        dictionary_item_ item{};
        item.first = first;
        item.second = U{};

        kvps_.push_back(item);

        return kvps_.back().second;
    }

public:
    const U &operator[](T first) const
    {
        return get(first);
    }

    U &operator[](T first)
    {
        return get(first);
    }

    /// @brief
    /// @return
    auto length() -> size_t
    {
        return kvps_.size();
    }

    /// @brief Count elements with a specific key. Searches the container for elements
    /// with a key equivalent to k and returns the number of matches.
    /// @param first
    /// @return
    auto count(T first) -> size_t
    {
        size_t n = 0;
        for (size_t i = 0; i < kvps_.size(); i++)
            if (kvps_.at(i).first == first)
                n++;
        return n;
    }

    auto clear() -> void
    {
        kvps_.clear();
    }

public:
    typedef map_iterator<T, U, MaxSize> iterator;

    iterator begin()
    {
        return iterator(&kvps_);
    }

    iterator end()
    {
        return iterator(&kvps_, kvps_.size());
    }

    typedef map_iterator<T, U, MaxSize> const_iterator;

    const_iterator begin() const
    {
        return const_iterator(&kvps_);
    }

    const_iterator end() const
    {
        return const_iterator(&kvps_, kvps_.size());
    }
};

}