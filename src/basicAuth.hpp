#include <Base64.h>

/// @brief
class BasicAuth
{
public:
//    static std::map<String, String> users;

public:
    static auto auth(Request &req, Response &res) -> bool
    {
        auto basicAuth = req.headers["authorization"]; // basic encodeUserPasswd

        if (basicAuth.startsWith(F("Basic")))
        {
            basicAuth = basicAuth.substring(6);
            basicAuth.trim();

            LOG_V(F("auth check"), F("Basic"), basicAuth);
        }

        return true;
    }
};

/// @brief
/// @return
static MiddlewareCallback basicAuth(const std::map<String, String> &users)
{
    return BasicAuth::auth;
}
