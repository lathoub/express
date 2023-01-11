#pragma once

// from Peter Polidoro peter@polidoro.io under MIT license
// https://github.com/janelia-arduino/vector
// (modified to fit into 1 header file)

template<typename T>
class vectorIterator
{
public:
  /// @brief 
  /// @param values_ptr 
  vectorIterator(T * values_ptr) : values_ptr_{values_ptr}, position_{0} {}

  /// @brief 
  /// @param values_ptr 
  /// @param size 
  vectorIterator(T * values_ptr, size_t size) : values_ptr_{values_ptr}, position_{size} {}

  /// @brief 
  /// @param other 
  /// @return 
  bool operator!=(const vectorIterator<T> & other) const
  {
    return !(*this == other);
  }

  /// @brief 
  /// @param other 
  /// @return 
  bool operator==(const vectorIterator<T> & other) const
  {
    return position_ == other.position_;
  }

  /// @brief 
  /// @return 
  vectorIterator & operator++()
  {
    ++position_;
    return *this;
  }

  /// @brief 
  /// @return 
  T & operator*() const
  {
    return *(values_ptr_ + position_);
  }

private:
  /// @brief 
  T * values_ptr_;
  /// @brief 
  size_t position_;
};

template <typename T>
class vector
{
public:
  /// @brief 
  vector()
  {
    values_ = NULL;
    max_size_ = 0;
    size_ = 0;
  };

  /// @brief 
  /// @tparam MAX_SIZE 
  /// @param values 
  /// @param size 
  template <size_t MAX_SIZE>
  vector(T (&values)[MAX_SIZE], size_t size = 0)
  {
    setStorage(values, size);
  };

  /// @brief 
  /// @tparam MAX_SIZE 
  /// @param values 
  /// @param size 
  template <size_t MAX_SIZE>
  void setStorage(T (&values)[MAX_SIZE], size_t size = 0)
  {
    values_ = values;
    max_size_ = MAX_SIZE;
    size_ = size;
  };

  /// @brief 
  /// @param values 
  /// @param max_size 
  /// @param size 
  void setStorage(T *values,
                  size_t max_size,
                  size_t size)
  {
    values_ = values;
    max_size_ = max_size;
    size_ = size;
  };

  /// @brief 
  /// @param index 
  /// @return 
  const T &operator[](size_t index) const
  {
    return values_[index];
  };

  /// @brief 
  /// @param index 
  /// @return 
  T &operator[](size_t index)
  {
    return values_[index];
  };

  /// @brief 
  /// @param index 
  /// @return 
  T &at(size_t index)
  {
    return values_[index];
  };

  /// @brief 
  /// @param index 
  /// @return 
  const T &at(size_t index) const
  {
    return values_[index];
  };

  /// @brief 
  /// @return 
  T &front()
  {
    return values_[0];
  };

  /// @brief 
  /// @return 
  T &back()
  {
    return values_[size_ - 1];
  };

  /// @brief 
  void clear()
  {
    size_ = 0;
  }

  /// @brief 
  /// @tparam U 
  /// @param value 
  template <typename U>
  void fill(const U &value)
  {
    assign(max_size_, value);
  }

  /// @brief 
  /// @tparam U 
  /// @tparam N 
  /// @param values 
  template <typename U,
            size_t N>
  void fill(const U (&values)[N])
  {
    assign(N, values);
  }

  /// @brief 
  /// @tparam U 
  /// @param values 
  template <typename U>
  void fill(const vector<U> &values)
  {
    assign(values.size(), values);
  }

  /// @brief 
  /// @tparam U 
  /// @param n 
  /// @param value 
  template <typename U>
  void assign(size_t n,
              const U &value)
  {
    size_t assign_size = ((n < max_size_) ? n : max_size_);
    size_ = assign_size;
    for (size_t i = 0; i < assign_size; ++i)
    {
      values_[i] = value;
    }
  }

  /// @brief 
  /// @tparam U 
  /// @tparam N 
  /// @param n 
  /// @param values 
  template <typename U,
            size_t N>
  void assign(size_t n,
              const U (&values)[N])
  {
    size_t n_smallest = ((n < N) ? n : N);
    size_t assign_size = ((n_smallest < max_size_) ? n_smallest : max_size_);
    size_ = assign_size;
    for (size_t i = 0; i < assign_size; ++i)
    {
      values_[i] = values[i];
    }
  }

  /// @brief 
  /// @tparam U 
  /// @param n 
  /// @param values 
  template <typename U>
  void assign(size_t n,
              const vector<U> &values)
  {
    size_t n_smallest = ((n < values.size()) ? n : values.size());
    size_t assign_size = ((n_smallest < max_size_) ? n_smallest : max_size_);
    size_ = assign_size;
    for (size_t i = 0; i < assign_size; ++i)
    {
      values_[i] = values[i];
    }
  }

  /// @brief 
  /// @param value 
  void push_back(const T &value)
  {
    if ((values_ != NULL) && (size_ < max_size_))
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
  /// @param index 
  void remove(size_t index)
  {
    if (size_ > index)
    {
      for (size_t i = index; i < (size_ - 1); ++i)
      {
        values_[i] = values_[i + 1];
      }
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
  size_t max_size() const
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

private:
  /// @brief 
  T *values_;

  /// @brief 
  size_t max_size_;

  /// @brief 
  size_t size_;
};
