/*!
 *  @file       Express.h
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

#pragma once

#include <Ethernet.h>

#define EX_DEBUG Serial

#include "utility/debug.h"
#include "defs.h"

#include "request.h"
#include "response.h"
#include "route.h"

BEGIN_EXPRESS_NAMESPACE

#define EXPRESS_LIBRARY_VERSION 0x000100
#define EXPRESS_LIBRARY_VERSION_MAJOR 0
#define EXPRESS_LIBRARY_VERSION_MINOR 1
#define EXPRESS_LIBRARY_VERSION_PATCH 0

using RenderEngineCallback = void (*)(EthernetClient &, locals_t &locals, const char *f);
using MiddlewareCallback = bool (*)(request &, response &);
using StartedCallback = void (*)();

struct ExpressDefaultSettings
{
    /// @brief
    static constexpr int MaxRoutes = 10;

    /// @brief
    static constexpr int MaxMountPaths = 2;

    /// @brief
    static constexpr int MaxMiddlewareCallbacks = 2;

    /// @brief
    static constexpr int MaxSettings = 4;

    /// @brief
    static constexpr int MaxEngines = 4;
};

// template <class ServerType, class ClientType, class _Settings = ExpressDefaultSettings>
class express
{
public:
    //  typedef _Settings Settings;

    friend class HttpRequestHandler;

private:
    /// @brief
    EthernetServer *server_{}; // TODO: singleton

private:
    /// @brief routes
    vector<Route *, 10> routes_{};

    /// @brief Application wide middlewares
    vector<MiddlewareCallback, 10> middlewares_{};

    /// @brief
    dictionary<String, express *, 10> mount_paths_{};

    /// @brief
    express *parent_ = nullptr;

public:
    /// @brief
    uint16_t port{};

    /// @brief Application Settings
    dictionary<String, String, 10> settings{};

    /// @brief The app.mountpath property contains the path patterns
    /// on which a sub-app was mounted.
    String mountpath{};

    /// @brief
    dictionary<String, RenderEngineCallback, 10> engines{};

private:
    // bodyparser

    // TODO: static options
    // inflate, limit, reviver, strict, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseJson(request &req, response &res) -> bool
    {
        EX_DBG_I(F("> bodyparser parseJson"));

        if (req.body != nullptr && req.body.length() > 0)
        {
            EX_DBG_I(F("Body already read"));
            return true;
        }

        auto &client = const_cast<EthernetClient &>(req.client_);

        if (req.get(ContentType).equalsIgnoreCase(ApplicationJson))
        {
            auto max_length = req.get(ContentLength).toInt();

            req.body.reserve(max_length);

            if (!req.body.reserve(max_length + 1))
                return false;

            req.body[0] = 0;

            while (req.body.length() < max_length)
            {
                int tries = 1000;
                size_t avail;

                while (!((avail = client.available())) && tries--)
                    delay(1);

                if (!avail)
                    break;

                if (req.body.length() + avail > max_length)
                    avail = max_length - req.body.length();

                while (avail--)
                    req.body += static_cast<char>(client.read());
            }

            res.headers_[ContentType] = ApplicationJson;

            EX_DBG_I(F("< bodyparser parseJson"));

            return true;
        }

        return true;
    }

    // TODO: static options
    // inflate, limit, type, verify

    /// @brief 
    /// @param req
    /// @param res
    /// @return
    static auto parseRaw(request &req, response &res) -> bool
    {
        EX_DBG_V(F("> bodyparser raw"));

        auto &client = const_cast<EthernetClient &>(req.client_);

        if (req.get(ContentType).equalsIgnoreCase(F("application/octet-stream")))
        {
            auto dataLen = req.get(ContentLength).toInt();

            while (dataLen > 0 && client.connected())
            {
                if (client.available())
                {
                    Buffer buffer;
                    buffer.length = client.read(buffer.buffer, sizeof(buffer.buffer));
                    dataLen -= buffer.length;

                    EX_DBG_V(F("remaining:"), buffer.length, dataLen);

                    if (dataLen > 0)
                    {
                        if (req.route_->dataCallback_)
                            req.route_->dataCallback_(buffer);
                    }
                    else
                    {
                        if (buffer.length > 0)
                        {
                            if (req.route_->dataCallback_)
                                req.route_->dataCallback_(buffer);
                        }
                        if (req.route_->endCallback_)
                            req.route_->endCallback_();
                    }
                }
            }
        }

        EX_DBG_V(F("< bodyparser raw"));

        return true;
    }

    // TODO: static options
    // defaultCharset, inflate, limit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseText(request &req, response &res) -> bool
    {
        return true;
    }

    // TODO: static options
    // extended, inflate, limit, parameterLimit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseUrlencoded(request &req, response &res) -> bool
    {
        return true;
    }

public:
    // bodyparser

    /// @brief
    /// @return
    static auto raw() -> MiddlewareCallback
    {
        return express::parseRaw;
    }

    /// @brief
    /// @return
    static auto json() -> MiddlewareCallback
    {
        return express::parseJson;
    }

    /// @brief
    /// @return
    static auto text() -> MiddlewareCallback
    {
        return express::parseText;
    }

    /// @brief
    /// @return
    static auto urlencoded() -> MiddlewareCallback
    {
        return express::parseUrlencoded;
    }

private:
    /// @brief
    /// @param path
    /// @param pathItems
    /// @param requestPath
    /// @param requestPathItems
    /// @param params
    /// @return
    static auto match(const String &path, const vector<PosLen> &pathItems,
                      const String &requestPath, const vector<PosLen> &requestPathItems,
                      params_t &params) -> bool
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
    /// @param req
    /// @param res
    auto evaluate(request &req, response &res) -> const bool
    {
        EX_DBG_V(F("evaluate"), req.uri_);

        vector<PosLen> req_indices{}; // TODO how many?? vis Settings

        Route::splitToVector(req.uri_, req_indices);

        for (auto route : routes_)
        {
            EX_DBG_V(F("req.method:"), req.method, F("method:"), route->method);
            EX_DBG_V(F("req.uri:"), req.uri_, F("path:"), route->path);

            if (req.method == route->method && match(route->path, route->indices,
                                                     req.uri_, req_indices,
                                                     req.params))
            {
                res.status_ = HttpStatus::OK;
                req.route_ = route;

                // Route middleware
                for (const auto handler : route->handlers)
                    if (!handler(req, res))
                        break;

                // evaluate the actual function
                if (route->fptrCallback)
                    route->fptrCallback(req, res);

                // go to the next middleware
                return true;
            }
        }

        // evaluate child mounting paths
        for (auto [mountPath, express] : mount_paths_)
            if (express->evaluate(req, res))
                return true;

        return false;
    }

    /// @brief
    /// @param method
    /// @param path
    /// @param handler
    /// @param fptrCallback
    /// @return
    auto METHOD(const Method method, String path, const HandlerCallback handler, const requestCallback fptrCallback) -> Route &
    {
        if (path == F("/"))
            path = F("");

        path = mountpath + path;

        EX_DBG_I(F("METHOD:"), method, F("mountpath:"), mountpath, F("path:"), path, F("handler:"), (nullptr == handler));

        const auto route = new Route();
        route->method = method;
        route->path = path;
        route->fptrCallback = fptrCallback;
        if (nullptr != handler)
            route->handlers.push_back(handler);
        route->splitToVector(route->path);
        // Add to collection
        routes_.push_back(route);

        return *route;
    }

    /// @brief
    /// @param method
    /// @param path
    /// @param fptr
    /// @return
    auto METHOD(const Method method, String path, const requestCallback fptr) -> Route &
    {
        EX_DBG_I(F("METHOD:"), method, F("mountpath:"), mountpath, F("path:"), path);
        return METHOD(method, path, nullptr, fptr);
    }

public:
    /// @brief Constructor
    express()
    {
        EX_DBG_V(F("express() constructor"));

        settings[F("env")] = F("production");
        //  settings[XPoweredBy] = F("X-Powered-By: Express for Arduino");
    }

    /// @brief
    /// @param middleware
    /// @return
    auto use(const MiddlewareCallback middleware) -> void
    {
        middlewares_.push_back(middleware);
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @param other
    /// @return
    auto use(const String &mount_path, express &other) -> void
    {
        EX_DBG_I(F("use mountPath:"), mount_path);

        other.mountpath = mount_path;
        other.parent_ = this;
        mount_paths_[other.mountpath] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    auto use(const String &mount_path) -> void
    {
        mountpath = mount_path;
    }

    /// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
    /// @param path
    /// @param fptr
    auto all(const String &path, const requestCallback fptr) -> void
    {
        // TODO: not implemented
    }

    /// @brief Sets the Boolean setting name to false, where name is one of the properties from
    /// the app settings table. Calling app.set('foo', false) for a Boolean property is the
    /// same as calling app.disable('foo').
    /// @param name
    auto disable(const String &name) -> void
    {
        settings[name] = False;
    }

    /// @brief Returns true if the Boolean setting name is disabled (false), where name is one
    /// of the properties from the app settings table.
    /// @param name
    /// @return
    auto disabled(const String &name) -> bool
    {
        return settings[name].equalsIgnoreCase(False);
    }

    /// @brief Sets the Boolean setting name to true, where name is one of the properties from the
    /// app settings table. Calling app.set('foo', true) for a Boolean property is the same as
    /// calling app.enable('foo').
    /// @param name
    auto enable(const String &name) -> void
    {
        settings[name] = True;
    }

    /// @brief Returns true if the setting name is enabled (true), where name is one of the
    /// properties from the app settings table.
    /// @param name
    /// @return
    auto enabled(const String &name) -> bool
    {
        return settings[name].equalsIgnoreCase(True);
    }

    /// @brief Returns the value of name app setting, where name is one of the strings in
    /// the app settings table.
    /// @param name
    /// @return
    auto get(const String &name) -> String
    {
        return settings[name];
    }

    /// @brief Assigns setting name to value. You may store any value that you want, but
    // certain names can be used to configure the behavior of the server. These special
    /// names are listed in the app settings table.
    /// @param name
    /// @param value
    auto set(const String &name, const String &value) -> void
    {
        settings[name] = value;
    }

    /// @brief register the given template engine callback as ext.
    /// @param name
    /// @param value
    auto engine(const String &ext, const RenderEngineCallback callback) -> void
    {
        engines[ext] = callback;
    }

#pragma region HTTP_Methods

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto get(const String &path, const requestCallback fptr) -> Route &
    {
        return METHOD(Method::GET, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto post(const String &path, const requestCallback fptr) -> Route &
    {
        return METHOD(Method::POST, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param middleware
    /// @param fptr
    /// @return
    auto post(const String &path, const MiddlewareCallback middleware, const requestCallback fptr) -> Route &
    {
        return METHOD(Method::POST, path, middleware, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto put(const String &path, const requestCallback fptr) -> Route &
    {
        return METHOD(Method::PUT, path, fptr);
    };

    /// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
    /// For more information, see the routing guide.
    /// @param path
    /// @param fptr
    auto Delete(const String &path, const requestCallback fptr) -> Route &
    {
        return METHOD(Method::DELETE, path, fptr);
    }

#pragma endregion HTTP_Methods

    /// @brief Returns the canonical path of the app, a string.
    /// @return
    auto path() -> String
    {
        // TODO: not sure
        return (parent_ == nullptr) ? mountpath : parent_->mountpath;
    }

    /// @brief Returns an instance of a single route, which you can then use to handle
    /// HTTP verbs with optional middleware. Use app.route() to avoid duplicate route names
    /// (and thus typo errors).
    auto route(const String &path) -> void
    {
        // TODO
    }

    /// @brief
    auto listen(uint16_t port, const StartedCallback startedCallback = nullptr) -> void
    {
        if (nullptr != server_)
        {
            EX_DBG_E(F("The listen method can only be called once! This call is ignored and processing continous."));
            return;
        }

        this->port = port;

        // Note: see https://github.com/PaulStoffregen/Ethernet/issues/42
        // change in ESP32 server.h
        // MacOS:   /Users/<user>/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
        // Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
        //      "virtual void begin(uint16_t port=0) =0;" to " virtual void begin() =0;"

        server_ = new EthernetServer(port);
        server_->begin();

        if (startedCallback)
            startedCallback();
    }

    /// @brief
    auto run() -> void
    {
        if (auto client = server_->available())
            run(client);
    }

    /// @brief
    /// @param client
    void run(EthernetClient &client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                // Construct request object and read/parse incoming bytes
                request req(*this, client);

                if (req.method != Method::ERROR)
                {
                    response res(*this, client);

                    /// @brief run the app wide middlewares (ao bodyparsers)
                    for (const auto middleware : middlewares_)
                        if (!middleware(req, res))
                            break;

                    /// @brief evaluate the request
                    evaluate(req, res);

                    res.send();
                }
            }
        }
    };
};

// typedef Express<EthernetServer, EthernetClient> express;

END_EXPRESS_NAMESPACE