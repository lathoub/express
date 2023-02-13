#include <Base64.h>

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief inspired by https://github.com/LionC/express-basic-auth
class BasicAuth {
public:
  static std::map<String, String> users;
  static bool challenge;

public:
  static auto auth(Request &req, Response &res, const NextCallback next)
      -> void {
    auto basicAuth = req.headers["authorization"]; // basic encodeUserPasswd

    LOG_V(F("BasicAuth::auth"), basicAuth);

    bool authenticated = false;
    if (basicAuth.startsWith(F("Basic"))) {
      basicAuth = basicAuth.substring(6);
      basicAuth.trim();

      for (auto const &user : users) {
        auto sum = user.first + ":" + user.second;
        if (base64::encode(sum) == basicAuth) {
          authenticated = true;
          break;
        }
      }
    }

    if (!authenticated) {
      LOG_V(F("FAILED AUTH"));
      if (challenge)
        res.set("WWW-Authenticate", "Basic");
      res.sendStatus(HttpStatus::DENIED);
      return;
    }

    next();
  }
};

std::map<String, String> BasicAuth::users{};
bool BasicAuth::challenge = false;

END_EXPRESS_NAMESPACE

/// @brief
/// @return
static MiddlewareCallback basicAuth(const std::map<String, String> &users,
                                    const bool challenge = true) {
  BasicAuth::users = users;
  BasicAuth::challenge = challenge;

  return BasicAuth::auth;
}
