#pragma once

// from Peter Polidoro peter@polidoro.io under MIT license
// https://github.com/janelia-arduino/vector
// (modified to fit into 1 header file)

/// @brief
/// @tparam T
template <typename T>
class vectorIterator
{
public:
    /// @brief
    /// @param values_ptr
    explicit vectorIterator(T *values_ptr)
        : values_ptr_{values_ptr}, position_{0}
    {
    }

    /// @brief
    /// @param values_ptr
    /// @param size
    vectorIterator(T *values_ptr, const size_t size)
        : values_ptr_{values_ptr}, position_{size}
    {
    }

    /// @brief
    /// @param other
    /// @return
    bool operator!=(const vectorIterator<T> &other) const
    {
        return !(*this == other);
    }

    /// @brief
    /// @param other
    /// @return
    bool operator==(const vectorIterator<T> &other) const
    {
        return position_ == other.position_;
    }

    /// @brief
    /// @return
    vectorIterator &operator++()
    {
        ++position_;
        return *this;
    }

    /// @brief
    /// @return
    T &operator*() const
    {
        return *(values_ptr_ + position_);
    }

private:
    T *values_ptr_;

    size_t position_;
};

template <typename T, size_t MaxSize = 10>
class vector
{
private:
    T values_[MaxSize];

    size_t size_;

public:
    vector()
    {
        size_ = 0;
    }

    const T &operator[](size_t index) const
    {
        return values_[index];
    }

    T &operator[](size_t index)
    {
        return values_[index];
    }

    T &at(size_t index)
    {
        return values_[index];
    }

    const T &at(size_t index) const
    {
        return values_[index];
    }

    T &front()
    {
        return values_[0];
    }

    T &back()
    {
        return values_[size_ - 1];
    }

    void clear()
    {
        size_ = 0;
    }

    void push_back(const T &value)
    {
        if (size_ < MaxSize)
            values_[size_++] = value;
    }

    void pop_back()
    {
        if (size_ > 0)
            --size_;
    }

    size_t size() const
    {
        return size_;
    }

    static size_t max_size()
    {
        return MaxSize;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    bool full() const
    {
        return size_ == MaxSize;
    }

    T *data()
    {
        return values_;
    }

    const T *data() const
    {
        return values_;
    }

    typedef vectorIterator<T> iterator;

    iterator begin()
    {
        return iterator(values_);
    }

    iterator end()
    {
        return iterator(values_, size_);
    }

    typedef vectorIterator<const T> const_iterator;

    const_iterator begin() const
    {
        return const_iterator(values_);
    }

    const_iterator end() const
    {
        return const_iterator(values_, size_);
    }
};
