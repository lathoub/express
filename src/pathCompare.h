#pragma once

#include <vector>

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class PathCompareAndExtractParams
{
private:
    static const char delimiter = '/';

public:
    /// @brief 
    /// @param path 
    /// @return 
    static auto splitToVector(const String &path) -> std::vector<PosLen>
    {
        std::vector<PosLen> pathItems;
        size_t p = 0, i = 1;
        for (; i < path.length(); i++)
        {
            if (path.charAt(i) == delimiter)
            {
                pathItems.push_back({p, i - p});
                p = i;
            }
        }
        pathItems.push_back({p, i - p});

        return pathItems;
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
        if (requestPathItems.size() != pathItems.size()) {
            EX_DBG(F("Items not equal. requestPathItems.size():"), requestPathItems.size(), F("pathItems.size():"), pathItems.size());
            EX_DBG(F("return false in function match"));
            return false;
        }

        for (size_t i = 0; i < requestPathItems.size(); i++)
        {
            const auto &ave = requestPathItems[i];
            const auto &bve = pathItems[i];

          //  EX_DBG(F("ave:"), ave, F("bve:"), bve);

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
