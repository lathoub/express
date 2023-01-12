#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

#include "request.h"
#include "response.h"

class Route
{
private:
    static const char delimiter = '/';

private:
    MiddlewareCallback saMiddlewareCallbacks_[maxMiddlewareCallbacks];
    PosLen saPosLens_[maxMiddlewareCallbacks];

public:
    Method method = Method::UNDEFINED;

    String path;

    std::vector<MiddlewareCallback> fptrMiddlewares;

    requestCallback fptrCallback = nullptr;

    // cache path splitting (avoid doing this for every request * number of paths)
    std::vector<PosLen> indices;

public:
    /// @brief
    Route()
    {
        EX_DBG_V(F("Route() constructor"));

//        fptrMiddlewares.setStorage(saMiddlewareCallbacks_);
  //      indices.setStorage(saPosLens_);
    }

    /// @brief 
    /// @param path 
    void splitToVector(const String &path)
    {
        splitToVector(path, indices);
    }

    /// @brief
    /// @param path
    /// @return
    static void splitToVector(const String &path, std::vector<PosLen> &poslens)
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
    static auto match(const String &path, const std::vector<PosLen> &pathItems,
                      const String &requestPath, const std::vector<PosLen> &requestPathItems,
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
};

END_EXPRESS_NAMESPACE
