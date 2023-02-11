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

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

class Express;
class Route;
class Request;
class Response;

using RenderEngineCallback = void (*)(EthernetClient &, locals_t &locals, const char *f);
using MiddlewareCallback = bool (*)(Request &, Response &);
using StartedCallback = void (*)();

using requestCallback = void (*)(Request &, Response &);

/// @brief 
class Express
{
private:
    /// @brief
    EthernetServer *server_{}; // TODO: singleton

    /// @brief routes
    std::vector<Route *> routes_;

    /// @brief Application wide middlewares
    std::vector<MiddlewareCallback> middlewares_;

    /// @brief
    std::map<String, Express *> mountPaths_;

    /// @brief
    Express *parent_ = nullptr;

public:
    /// @brief Constructor
    Express();

    /// @brief
    uint16_t port{};

    /// @brief Application Settings
    std::map<String, String> settings;

    /// @brief The app.mountpath property contains the path patterns
    /// on which a sub-app was mounted.
    String mountpath{};

    /// @brief
    std::map<String, RenderEngineCallback> engines;

#pragma region express

private:
    // bodyparser

    // TODO: static options
    // inflate, limit, reviver, strict, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseJson(Request &req, Response &res) -> bool;

    // TODO: static options
    // inflate, limit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseRaw(Request &req, Response &res) -> bool;

    // TODO: static options
    // defaultCharset, inflate, limit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseText(Request &req, Response &res) -> bool;

    // TODO: static options
    // extended, inflate, limit, parameterLimit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseUrlencoded(Request &req, Response &res) -> bool;

    /// @brief This is a built-in middleware function in Express. It serves static files and is based on serve-static.
    //static void Static() {}

public:
    /// @brief
    /// @return
    static auto raw() -> MiddlewareCallback
    {
        return Express::parseRaw;
    }

    /// @brief
    /// @return
    static auto json() -> MiddlewareCallback
    {
        return parseJson;
    }

    /// @brief
    /// @return
    static auto text() -> MiddlewareCallback
    {
        return parseText;
    }

    /// @brief
    /// @return
    static auto urlencoded() -> MiddlewareCallback
    {
        return parseUrlencoded;
    }

#pragma endregion express

private:
    /// @brief
    /// @param path
    /// @param pathItems
    /// @param requestPath
    /// @param requestPathItems
    /// @param params
    /// @return
    static auto match(const String &path, const std::vector<PosLen> &pathItems,
                      const String &requestPath, const std::vector<PosLen> &requestPathItems,
                      params_t &params) -> bool
    {
        if (requestPathItems.size() != pathItems.size())
        {
            LOG_V(F("Items not equal. requestPathItems.size():"), requestPathItems.size(), F("pathItems.size():"), pathItems.size());
            LOG_V(F("return false in function match"));
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
                if (requestPath.substring(ave.pos, ave.pos + ave.len) != path.substring(bve.pos, bve.pos + bve.len)) {
                    return false;
                }
            }
        }

        return true;
    }

    /// @brief
    /// @param req
    /// @param res
    auto evaluate(Request &req, Response &res) -> const bool;

    /// @brief
    /// @param method
    /// @param path
    /// @param handler
    /// @param fptrCallback
    /// @return
    template <typename T, size_t N>
    auto METHOD(const Method method, String path, const T (&handlers)[N], const requestCallback fptrCallback) -> Route  &;

    /// @brief
    /// @param method
    /// @param path
    /// @param fptr
    /// @return
    auto METHOD(const Method method, String path, const requestCallback fptr) -> Route  &;

public:
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
    auto use(const String &mount_path, Express &other) -> void
    {
        LOG_I(F("use mountPath:"), mount_path);

        other.mountpath = mount_path;
        other.parent_ = this;
        mountPaths_[other.mountpath] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    auto use(const String &mount_path) -> void
    {
        mountpath = mount_path;
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
    auto head(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::HEAD, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto get(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::GET, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto post(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::POST, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param middleware
    /// @param fptr
    /// @return
    auto post(const String &path, const MiddlewareCallback middleware, const requestCallback fptr = nullptr) -> Route  &
    {
        const MiddlewareCallback middlewares[] = {middleware};
        return METHOD(Method::POST, path, middlewares, fptr);
    };

    /// @brief
    /// @param path
    /// @param middleware
    /// @param fptr
    /// @return
    template <typename T, size_t N>
    auto post(const String &path, const T (&middlewares)[N], const requestCallback fptr = nullptr) -> Route  &
    {
        return METHOD(Method::POST, path, middlewares, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto put(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::PUT, path, fptr);
    };

    /// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
    /// For more information, see the routing guide.
    /// @param path
    /// @param fptr
    auto Delete(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::DELETE, path, fptr);
    }

    /// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
    /// @param path
    /// @param fptr
    auto all(const String &path, const requestCallback fptr) -> Route  &
    {
        return METHOD(Method::ALL, path, fptr);
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
    void listen(uint16_t port, const StartedCallback startedCallback = nullptr);

    /// @brief
    auto run() -> void;

    /// @brief
    /// @param client
    void run(EthernetClient &client);
};

END_EXPRESS_NAMESPACE

#include "response.h"
#include "request.h"
#include "route.h"

#define EXPRESS_CREATE_NAMED_INSTANCE(Name) \
    typedef Express express; \
    typedef Route route; \
    typedef Request request; \
    typedef Response response; \
    express Name;

#define EXPRESS_CREATE_INSTANCE() \
    EXPRESS_CREATE_NAMED_INSTANCE(app);
