#pragma once

// from Peter Polidoro peter@polidoro.io under MIT license
// https://github.com/janelia-arduino/vector
// (modified to fit into 1 header file)

template <typename T>
class vectorIterator
{
public:
    /// @brief
    /// @param values_ptr
    explicit vectorIterator(T *values_ptr) : values_ptr_{values_ptr}, position_{0} {}

    /// @brief
    /// @param values_ptr
    /// @param size
    vectorIterator(T *values_ptr, const size_t size) : values_ptr_{values_ptr}, position_{size} {}

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
    /// @brief points to the holder of the values
    T *values_ptr_;

    /// @brief current element in the values array
    size_t position_;
};

template <typename T, size_t max_size_ = 10>
class vector
{
private:
    T values_[max_size_];

    size_t size_;

public:
    /// @brief constructor
    vector()
    {
        size_ = 0;
    }

    /// @brief
    /// @param index
    /// @return
    const T &operator[](size_t index) const
    {
        return values_[index];
    }

    /// @brief
    /// @param index
    /// @return
    T &operator[](size_t index)
    {
        return values_[index];
    }

    /// @brief
    /// @param index
    /// @return
    T &at(size_t index)
    {
        return values_[index];
    }

    /// @brief
    /// @param index
    /// @return
    const T &at(size_t index) const
    {
    	return values_[index];
    }

    /// @brief
    /// @return
    T &front()
    {
        return values_[0];
    }

    /// @brief
    /// @return
    T &back()
    {
        return values_[size_ - 1];
    }

    /// @brief
    void clear()
    {
        size_ = 0;
    }

    /// @brief
    /// @param value
    void push_back(const T &value)
    {
        if ((values_ != nullptr) && (size_ < max_size_))
        {
            values_[size_++] = value;
        }
    }

    /// @brief
    void pop_back()
    {
        if (size_ > 0)
        {
            --size_;
        }
    }

    /// @brief
    /// @return
    size_t size() const
    {
        return size_;
    }

    /// @brief
    /// @return
    static size_t max_size()
    {
        return max_size_;
    }

    /// @brief
    /// @return
    bool empty() const
    {
        return size_ == 0;
    }

    /// @brief
    /// @return
    bool full() const
    {
        return size_ == max_size_;
    }

    /// @brief
    /// @return
    T *data()
    {
        return values_;
    }

    /// @brief
    /// @return
    const T *data() const
    {
        return values_;
    }





    /// @brief
    typedef vectorIterator<T> iterator;

    /// @brief
    /// @return
    iterator begin()
    {
        return iterator(values_);
    }

    /// @brief
    /// @return
    iterator end()
    {
        return iterator(values_, size_);
    }

    /// @brief
    typedef vectorIterator<const T> const_iterator;

    /// @brief
    /// @return
    const_iterator begin() const
    {
        return const_iterator(values_);
    }

    /// @brief
    /// @return
    const_iterator end() const
    {
        return const_iterator(values_, size_);
    }
};
