#include "vector.h"

#define _MAXX_ELEMENTS 10

template <class T, class U>
class dictionary
{
private:
    T saKeys_[_MAXX_ELEMENTS];
    T saValues_[_MAXX_ELEMENTS];

    vector<T> keys_{};
    vector<U> values_{};

public:
    /// @brief
    dictionary()
    {
        keys_.setStorage(saKeys_);
        values_.setStorage(saValues_);
    }

    /// @brief
    /// @param
    /// @return
    U &operator[](T target)
    {
        int index = 0;
        for (auto key : keys_)
        {
            index++;
            if (key == target) // Case sensitivity
                return values_.at(index);
        }

        return nullptr;
    }

    /// @brief
    /// @param target
    /// @return
    dictionary &operator=(T target)
    {
        return *this;
    }

    /// @brief
    operator T() { return T(); }

    /// @brief
    /// @return
    size_t size() const
    {
        return keys_.size();
    }
};
