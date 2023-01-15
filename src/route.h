#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

#include "request.h"
#include "response.h"

class Route
{
    friend class express;

private:
    static const char delimiter = '/';

private: /// @brief
    DataCallback dataCallback_ = nullptr;

    /// @brief
    EndDataCallback endCallback_ = nullptr;

public:
    Method method = Method::UNDEFINED;

    String path = F("");

    vector<HandlerCallback, 10> handlers{};

    requestCallback fptrCallback = nullptr;

    // cache path splitting (avoid doing this for every request * number of paths)
    vector<PosLen, 10> indices{};

public:
    /// @brief constructor
    Route()
    {
        EX_DBG_V(F("Route() constructor"));
    }

    /// @brief
    /// @param path
    auto splitToVector(const String& path) -> void
    {
        splitToVector(path, indices);
    }

    /// @brief
    /// @param path
    /// @return
    static auto splitToVector(const String& path, vector<PosLen, 10>& poslens) -> void
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
    /// @param path
    /// @param pathItems
    /// @param requestPath
    /// @param requestPathItems
    /// @param params
    /// @return
    static auto match(const String &path, const vector<PosLen, 10> &pathItems,
                      const String &requestPath, const vector<PosLen, 10> &requestPathItems,
                      std::map<String, String> &params) -> bool
    {
        if (requestPathItems.size() != pathItems.size())
        {
            EX_DBG_I(F("Items not equal. requestPathItems.size():"), requestPathItems.size(), F("pathItems.size():"), pathItems.size());
            EX_DBG_I(F("return false in function match"));
            return false;
        }

        for (size_t i = 0; i < requestPathItems.size(); i++)
        {
            const auto &ave = requestPathItems[i];
            const auto &bve = pathItems[i];

            if (path.charAt(bve.pos + 1) == ':') // Note: : comes right after /
            {
                auto name = path.substring(bve.pos + 2, bve.pos + bve.len); // Note: + 2 to offset /:
                name.toLowerCase();
                const auto value = requestPath.substring(ave.pos + 1, ave.pos + ave.len); // Note + 1 to offset /
                params[name] = value;
            }
            else
            {
                if (requestPath.substring(ave.pos, ave.pos + ave.len) != path.substring(bve.pos, bve.pos + bve.len))
                    return false;
            }
        }

        return true;
    }

    /// @brief
    /// @param name
    /// @param callback
    auto on(const String& name, const DataCallback callback) -> void
    {
        EX_DBG_I(F("register data callback"), name);
        dataCallback_ = callback;
        // return *this;
    }

    /// @brief
    /// @param name
    /// @param callback
    auto on(const String& name, const EndDataCallback callback) -> void
    {
        EX_DBG_I(F("register end callback"), name);
        endCallback_ = callback;
        //  return *this;
    }
};

END_EXPRESS_NAMESPACE
