/*!
 *  @file       _Express.cpp
 *  Project     Arduino Express Library
 *  @brief      Fast, unopinionated, (very) minimalist web framework for Arduino
 *  @author     lathoub
 *  @date       20/01/23
 *  @license    GNU GENERAL PUBLIC LICENSE
 *
 *   Fast, unopinionated, (very) minimalist web framework for Arduino.
 *   Copyright (C) 2023 lathoub
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Express.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief
/// @return
_Express::_Express() {
  LOG_T(F("Express constructor"));

  set(F("env"), F("production"));
  // https://expressjs.com/en/guide/behind-proxies.html
  disable(F("trust proxy")); // default is false
  //  settings[XPoweredBy] = F("X-Powered-By: _Express for Arduino");

  LOG_I(F("booting in"), settings[F("env")], F("mode"));

  router_ = new _Router();

  mountpath = F(""); // TODO: check: could also be /
}

#pragma region _Express()

/// @brief
/// @param req
/// @param res
/// @return
auto _Express::parseJson(_Request &req, _Response &res, const NextCallback next)
    -> void {
  if (req.body != nullptr && req.body.length() > 0) {
    LOG_I(F("Body already read"));
    next(nullptr);
    return;
  }

  if (req.get(ContentType).equalsIgnoreCase(ApplicationJson)) {
    LOG_I(F("> bodyparser parseJson"));

    auto max_length = req.get(ContentLength).toInt();

    req.body.reserve(max_length);
    if (!req.body.reserve(max_length + 1)) {
      return; // error
    }

    req.body[0] = 0;

    while (req.body.length() < max_length) {
      int tries = 1000;
      size_t avail;

      while (!((avail = req.client.available())) && tries--)
        delay(1);

      if (!avail)
        break;

      if (req.body.length() + avail > max_length)
        avail = max_length - req.body.length();

      while (avail--)
        req.body += static_cast<char>(req.client.read());
    }

    res.headers[ContentType] = ApplicationJson;

    LOG_I(F("< bodyparser parseJson"));
  } else
    LOG_V(F("Not an application/json body"));

  next(nullptr);
}

/// @brief
/// @param req
/// @param res
/// @return
auto _Express::parseRaw(_Request &req, _Response &res, const NextCallback next)
    -> void {
  if (req.body != nullptr && req.body.length() > 0) {
    LOG_I(F("Body already read"));
    next(nullptr);
    return;
  }

  if (req.get(ContentType).equalsIgnoreCase(F("application/octet-stream"))) {
    LOG_I(F("> bodyparser raw"));

    auto sDataLen = req.get("content-length");
    LOG_I(F("sDataLen"), sDataLen);

    auto dataLen = sDataLen.toInt();

    LOG_V(F("> contentLength"), dataLen);

    while (dataLen > 0 && req.client.connected()) {
      if (req.client.available()) {
        Buffer buffer;
        buffer.length = req.client.read(buffer.buffer, sizeof(buffer.buffer));
        dataLen -= buffer.length;

        LOG_V(F("remaining:"), buffer.length, dataLen);

        if (dataLen > 0) {
          if (req.route->dataCallback_)
            req.route->dataCallback_(buffer);
        } else {
          if (buffer.length > 0) {
            if (req.route->dataCallback_)
              req.route->dataCallback_(buffer);
          }
          if (req.route->endCallback_)
            req.route->endCallback_();
        }
      }
    }

    LOG_V(F("< bodyparser raw"));
  } else
    LOG_V(F("Not an application/octet-stream body"));

  next(nullptr);
}

/// @brief
/// @param req
/// @param res
/// @return
auto _Express::parseText(_Request &req, _Response &res, const NextCallback next)
    -> void {
  if (req.body != nullptr && req.body.length() > 0) {
    LOG_I(F("Body already read"));
    next(nullptr);
    return;
  }
}

/// @brief
/// @param req
/// @param res
/// @return
auto _Express::parseUrlencoded(_Request &req, _Response &res,
                              const NextCallback next) -> void {
  if (req.body != nullptr && req.body.length() > 0) {
    LOG_I(F("Body already read"));
    next(nullptr);
    return;
  }

  if (req.get(ContentType)
          .equalsIgnoreCase(F("application/x-www-form-urlencoded"))) {
    LOG_I(F("> bodyparser x-www-form-urlencoded"));
  } else
    LOG_V(F("Not an application/x-www-form-urlencoded body"));

  next(nullptr);
}

/// @brief
/// @return a MiddlewareCallback
auto _Express::raw() -> MiddlewareCallback { return _Express::parseRaw; }

/// @brief This is a built-in middleware function in _Express.
/// It parses incoming requests with JSON payloads and is based on body-parser.
/// @return Returns middleware that only parses JSON and only looks at requests
/// where the Content-Type header matches the type option.
auto _Express::json() -> MiddlewareCallback { return parseJson; }

/// @brief
/// @return a MiddlewareCallback
auto _Express::text() -> MiddlewareCallback { return parseText; }

/// @brief This is a built-in middleware function in _Express. It parses incoming
/// requests with urlencoded payloads and is based on body-parser.
///
/// @return Returns middleware that only parses urlencoded bodies and only looks
/// at requests where the Content-Type header matches the type option. This
/// parser accepts only UTF-8 encoding of the body and supports automatic
/// inflation of gzip and deflate encodings.
auto _Express::urlencoded() -> MiddlewareCallback { return parseUrlencoded; }

/// @brief Creates a new _Router object.
auto _Express::Router() -> _Router & {
  const auto _router = new _Router();
  return *_router;
}

#pragma endregion _Express

#pragma region Middleware

/// @brief
/// @param middleware
/// @return
auto _Express::use(const ErrorCallback errorCallback) -> void {
  router_->use(errorCallback);
}

/// @brief
/// @param middleware
/// @return
auto _Express::use(const std::vector<ErrorCallback> errorCallbacks) -> void {
  router_->use(errorCallbacks);
}

/// @brief
/// @param middleware
/// @return
auto _Express::use(const MiddlewareCallback middleware) -> void // TODO, args...
{
  router_->use(middleware);
}

/// @brief
/// @param middleware
/// @return
auto _Express::use(const std::vector<MiddlewareCallback> middlewares)
    -> void // TODO, args...
{
  router_->use(middlewares);
}

/// @brief
/// @param middleware
/// @return
auto _Express::use(const String &path, const MiddlewareCallback middleware)
    -> void // TODO, args...
{
  router_->use(path, middleware);
}

/// @brief The app.mountpath property contains one or more path patterns on
/// which a sub-app was mounted.
/// @param mountpath
/// @param otherRouter
/// @return
auto _Express::use(const String &mountpath, _Router &otherRouter) -> void {
  router_->use(mountpath, otherRouter);
}

/// @brief The app.mountpath property
/// @param mountpath
/// @return
auto _Express::use(const String &mountpath) -> void { router_->use(mountpath); }

#pragma endregion Middleware

/// @brief Returns the canonical path of the app, a string.
/// @return
auto _Express::path() -> String { return mountpath; }

/// @brief Returns an instance of a single route, which you can then use to
/// handle HTTP verbs with optional middleware. Use app.route() to avoid
/// duplicate route names (and thus typo errors).
auto _Express::route(const String &path) -> _Route & {
  return router_->route(path);
}

#pragma region Events

/// @brief
/// @param name
/// @param callback
auto _Express::on(const String &name, const MountCallback callback) -> void {}

#pragma endregion Events

/// @brief
/// @param port
/// @param startedCallback
/// @return
void _Express::listen(uint16_t port, const Callback startedCallback) {
  if (nullptr != server) {
    LOG_E(F("The listen method can only be called once! This call is ignored "
            "and processing continous."));
    return;
  }

  this->port = port;

  // Note: see https://github.com/PaulStoffregen/Ethernet/issues/42
  // change in ESP32 server.h
  // MacOS:
  // /Users/<user>/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
  // Windows:
  // C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
  //      "virtual void begin(uint16_t port=0) =0;" to " virtual void begin()
  //      =0;"

#if PLATFORM == ESP32
  server = new ServerType(port);
  server->begin();
#endif

#if PLATFORM == ESP32_W5500
  server = new ServerType(port);
  server->begin();
#endif

  if (startedCallback)
    startedCallback();
}

/// @brief
/// @return
auto _Express::run() -> void {
  if (auto client = server->available())
    run(client);
}

/// @brief
/// @param client
void _Express::run(ClientType &client) {
  while (client.connected()) {
    if (client.available()) {
      // Construct request object and read/parse incoming bytes
      _Request req(*this, client);

      if (req.method != Method::ERROR) {
        _Response res(*this, client);

        router_->dispatch(req, res);

        res.send();
      }

      // Arduino Ethernet stop() is potentially slow, this makes it faster
#if PLATFORM == ESP32_W5500
      client.setConnectionTimeout(5);
#endif
      client.stop();
    }
  }
};

END_EXPRESS_NAMESPACE
