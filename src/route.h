#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

#include "request.h"
#include "response.h"

class Routes : public std::vector<Route>
{
private:
public:

    bool evaluate(Request &req, Response &res)
    {
        res.body_ = "";
        res.status_ = HTTP_STATUS_NOT_FOUND;
        res.headers_.clear();
        // const auto req_indices = PathCompareAndExtractParams::splitToVector(req.uri_);

        return true;
    }
};

class Route
{
private:
    static const char delimiter = '/';

public:
    Method method = Method::UNDEFINED;
    String path{};
    std::list<MiddlewareCallback> fptrMiddlewares{};
    requestCallback fptrCallback = nullptr;
    // cache path splitting (avoid doing this for every request * number of paths)
    std::vector<PosLen> indices{};

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

    bool evaluate(Request &req, Response &res)
    {
        EX_DBG_I(F("req.method:"), req.method, F("method:"), method);
        EX_DBG_I(F("req.uri:"), req.uri_, F("path:"), path);
        /*
                if (req.method == method && PathCompareAndExtractParams::match(
                                                path, indices,
                                                req.uri_, req_indices,
                                                req.params))
                {
                    res.status_ = HTTP_STATUS_OK; // assumes all goes OK

                    auto it = fptrMiddlewares.begin();
                    while (it != fptrMiddlewares.end())
                    {
                        if ((*it)(req, res))
                            ++it;
                        else
                            break;
                    }

                    if (fptrCallback)
                        fptrCallback(req, res);

                    return true;
                }
                */
        return true;
    }
};

END_EXPRESS_NAMESPACE
