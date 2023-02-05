#pragma once

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

using HandlerCallback = bool (*)(request &, response &);
using requestCallback = void (*)(request &, response &);

template <class T = int, class U = int, class Settings = DefaultSettings>
class Route
{
    using DataCallback = void (*)(const Buffer &);
    using EndDataCallback = void (*)();

private:
    static const char delimiter = '/';

public: /// @brief
    DataCallback dataCallback_ = nullptr;

    /// @brief
    EndDataCallback endCallback_ = nullptr;

public:
    Method method = Method::UNDEFINED;

    String path{};

    vector<HandlerCallback, 5> handlers{}; // TODO how many

    requestCallback fptrCallback = nullptr;

    // cache path splitting (avoid doing this for every request * number of paths)
    vector<PosLen> indices{}; // TODO how many

public:
    /// @brief
    /// @param path
    auto splitToVector(const String &path) -> void
    {
        splitToVector(path, indices);
    }

    /// @brief
    /// @param path
    /// @return
    static auto splitToVector(const String &path, vector<PosLen> &poslens) -> void
    {
        size_t p = 0, i = 1;
        for (; i < path.length(); i++)
        {
            if (path.charAt(i) == delimiter)
            {
                poslens.push_back({p, i - p});
                p = i;
            }
        }
        poslens.push_back({p, i - p});
    }

    /// @brief
    /// @param name
    /// @param callback
    auto on(const String &name, const DataCallback callback) -> void
    {
        LOG_I(F("register data callback"), name);
        dataCallback_ = callback;
        // return *this;
    }

    /// @brief
    /// @param name
    /// @param callback
    auto on(const String &name, const EndDataCallback callback) -> void
    {
        LOG_I(F("register end callback"), name);
        endCallback_ = callback;
        //  return *this;
    }
};

//typedef Route<> route;

END_EXPRESS_NAMESPACE
