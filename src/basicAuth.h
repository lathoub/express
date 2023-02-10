#include <Base64.h>

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief
class BasicAuth
{
public:
    static std::map<String, String> users;
    static bool challenge;

public:
    static auto auth(Request &req, Response &res) -> bool
    {
        auto basicAuth = req.headers["authorization"]; // basic encodeUserPasswd

        LOG_V(F("BasicAuth::auth"), basicAuth);

        if (basicAuth.startsWith(F("Basic")))
        {
            basicAuth = basicAuth.substring(6);
            basicAuth.trim();

            bool match = false;
            for (auto const &user : users)
            {
                auto sum = user.first + ":" + user.second;
                if (base64::encode(sum) == basicAuth)
                {
                    match = true;
                    break;
                }
            }

            if (!match) {
                LOG_V(F("FAILED AUTH"));
                res.sendStatus(HttpStatus::DENIED);
                return false;
            }
        }

        LOG_V(F("OK AUTH"));
        return true;
    }
};

std::map<String, String> BasicAuth::users{};
bool BasicAuth::challenge = false;

END_EXPRESS_NAMESPACE

/// @brief
/// @return
static MiddlewareCallback basicAuth(const std::map<String, String> &users)
{
    BasicAuth::users = users;

    return BasicAuth::auth;
}
