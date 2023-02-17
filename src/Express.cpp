/*!
 *  @file       express.cpp
 *  Project     Arduino express Library
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

#include "express.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief
/// @return
express::express() {
  LOG_T(F("express constructor"));

  settings[F("env")] = F("production");
  //  settings[XPoweredBy] = F("X-Powered-By: express for Arduino");

  LOG_I(F("booting in"), settings[F("env")], F("mode"));

  router_ = new router();

  mountpath = F(""); // TODO: check: could also be /
}

#pragma region express()

/// @brief
/// @param req
/// @param res
/// @return
auto express::parseJson(Request &req, Response &res, const NextCallback next)
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
auto express::parseRaw(Request &req, Response &res, const NextCallback next)
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
auto express::parseText(Request &req, Response &res, const NextCallback next)
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
auto express::parseUrlencoded(Request &req, Response &res,
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
auto express::raw() -> MiddlewareCallback { return express::parseRaw; }

/// @brief This is a built-in middleware function in express.
/// It parses incoming requests with JSON payloads and is based on body-parser.
/// @return Returns middleware that only parses JSON and only looks at requests
/// where the Content-Type header matches the type option.
auto express::json() -> MiddlewareCallback { return parseJson; }

/// @brief
/// @return a MiddlewareCallback
auto express::text() -> MiddlewareCallback { return parseText; }

/// @brief This is a built-in middleware function in express. It parses incoming
/// requests with urlencoded payloads and is based on body-parser.
///
/// @return Returns middleware that only parses urlencoded bodies and only looks
/// at requests where the Content-Type header matches the type option. This
/// parser accepts only UTF-8 encoding of the body and supports automatic
/// inflation of gzip and deflate encodings.
auto express::urlencoded() -> MiddlewareCallback { return parseUrlencoded; }

/// @brief Creates a new router object.
auto express::Router() -> router & {
  const auto _router = new router();
  return *_router;
}

#pragma endregion express

#pragma region Middleware

/// @brief
/// @param middleware
/// @return
auto express::use(const ErrorCallback errorCallback) -> void 
{
  router_->use(errorCallback);
}

/// @brief
/// @param middleware
/// @return
auto express::use(const std::vector<ErrorCallback> errorCallbacks) -> void 
{
  router_->use(errorCallbacks);
}

/// @brief
/// @param middleware
/// @return
auto express::use(const MiddlewareCallback middleware) -> void // TODO, args...
{
  router_->use(middleware);
}

/// @brief
/// @param middleware
/// @return
auto express::use(const std::vector<MiddlewareCallback> middlewares)
    -> void // TODO, args...
{
  router_->use(middlewares);
}

/// @brief
/// @param middleware
/// @return
auto express::use(const String &path, const MiddlewareCallback middleware)
    -> void // TODO, args...
{
  router_->use(path, middleware);
}

/// @brief The app.mountpath property contains one or more path patterns on
/// which a sub-app was mounted.
/// @param mountpath
/// @param otherRouter
/// @return
auto express::use(const String &mountpath, router &otherRouter) -> void {
  router_->use(mountpath, otherRouter);
}

/// @brief The app.mountpath property
/// @param mountpath
/// @return
auto express::use(const String &mountpath) -> void { router_->use(mountpath); }

#pragma endregion Middleware

/// @brief Returns the canonical path of the app, a string.
/// @return
auto express::path() -> String { return mountpath; }

/// @brief Returns an instance of a single route, which you can then use to
/// handle HTTP verbs with optional middleware. Use app.route() to avoid
/// duplicate route names (and thus typo errors).
auto express::route(const String &path) -> Route & {
  return router_->route(path);
}

#pragma region Events

/// @brief
/// @param name
/// @param callback
auto express::on(const String &name, const MountCallback callback) -> void {}

#pragma endregion Events

/// @brief
/// @param port
/// @param startedCallback
/// @return
void express::listen(uint16_t port, const Callback startedCallback) {
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

  server = new ServerType(port);
  server->begin();

  if (startedCallback)
    startedCallback();
}

/// @brief
/// @return
auto express::run() -> void {
  if (auto client = server->available())
    run(client);
}

/// @brief
/// @param client
void express::run(ClientType &client) {
  while (client.connected()) {
    if (client.available()) {
      // Construct request object and read/parse incoming bytes
      Request req(*this, client);

      if (req.method != Method::ERROR) {
        Response res(*this, client);

        router_->dispatch(req, res);

        res.send();
      }

      // Arduino Ethernet stop() is potentially slow, this makes it faster
      if (true) {
        client.setConnectionTimeout(5);
        client.stop();
      }
    }
  }
};

END_EXPRESS_NAMESPACE
