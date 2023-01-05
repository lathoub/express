#pragma once

#include <vector>

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class PathCompareAndExtractParams
{
private:
    static const char delimiter = '/';

public:
    static auto splitToVector(const String &path) -> std::vector<PosLen>
    {
        std::vector<PosLen> v;
        size_t p = 0, i = 1;
        for (; i < path.length(); i++)
        {
            if (path.charAt(i) == delimiter)
            {
                v.push_back({p, i - p});
                p = i;
            }
        }
        v.push_back({p, i - p});

        return v;
    }

    static auto match(const String &b, const std::vector<PosLen> &bv,
                      const String &a, const std::vector<PosLen> &av,
                      std::map<String, String> &params) -> bool
    {
        if (av.size() != bv.size())
            return false;

        for (size_t i = 0; i < av.size(); i++)
        {
            const auto &ave = av[i];
            const auto &bve = bv[i];

            if (b.charAt(bve.pos + 1) == ':') // Note: : comes right after /
            {
                const auto &name = b.substring(bve.pos + 2, bve.pos + bve.len);  // Note: + 2 to offset /:
                const auto &value = a.substring(ave.pos + 1, ave.pos + ave.len); // Note +1 to offset /
                params[name] = value;
            }
            else
            {
                if (a.substring(ave.pos, ave.pos + ave.len) != b.substring(bve.pos, bve.pos + bve.len))
                    return false;
            }
        }

        return true;
    }
};

END_EXPRESS_NAMESPACE
