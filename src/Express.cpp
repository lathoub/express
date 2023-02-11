/*!
 *  @file       Express.cpp
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
Express::Express()
{
    LOG_V(F("express() constructor"));

    settings[F("env")] = F("production");
    //  settings[XPoweredBy] = F("X-Powered-By: Express for Arduino");
}

/// @brief
/// @param req
/// @param res
/// @return
auto Express::parseJson(Request &req, Response &res) -> bool
{
    if (req.body != nullptr && req.body.length() > 0)
    {
        LOG_I(F("Body already read"));
        return true;
    }

    if (req.get(ContentType).equalsIgnoreCase(ApplicationJson))
    {
        LOG_I(F("> bodyparser parseJson"));

        auto max_length = req.get(ContentLength).toInt();

        req.body.reserve(max_length);

        if (!req.body.reserve(max_length + 1))
            return false;

        req.body[0] = 0;

        while (req.body.length() < max_length)
        {
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

        return true;
    }
    else
        LOG_V(F("Not an application/json body"));

    return true;
}

/// @brief
/// @param req
/// @param res
/// @return
auto Express::parseRaw(Request &req, Response &res) -> bool
{
    if (req.body != nullptr && req.body.length() > 0)
    {
        LOG_I(F("Body already read"));
        return true;
    }

    if (req.get(ContentType).equalsIgnoreCase(F("application/octet-stream")))
    {
        LOG_I(F("> bodyparser raw"));

        auto sDataLen = req.get("content-length");
        LOG_I(F("sDataLen"), sDataLen);

        auto dataLen = sDataLen.toInt();

        LOG_V(F("> contentLength"), dataLen);

        while (dataLen > 0 && req.client.connected())
        {
            if (req.client.available())
            {
                Buffer buffer;
                buffer.length = req.client.read(buffer.buffer, sizeof(buffer.buffer));
                dataLen -= buffer.length;

                LOG_V(F("remaining:"), buffer.length, dataLen);

                if (dataLen > 0)
                {
                    if (req.route->dataCallback_)
                        req.route->dataCallback_(buffer);
                }
                else
                {
                    if (buffer.length > 0)
                    {
                        if (req.route->dataCallback_)
                            req.route->dataCallback_(buffer);
                    }
                    if (req.route->endCallback_)
                        req.route->endCallback_();
                }
            }
        }

        LOG_V(F("< bodyparser raw"));
    }
    else
        LOG_V(F("Not an application/octet-stream body"));

    return true;
}

/// @brief
/// @param req
/// @param res
/// @return
auto Express::parseText(Request &req, Response &res) -> bool
{
    if (req.body != nullptr && req.body.length() > 0)
    {
        LOG_I(F("Body already read"));
        return true;
    }

    return true;
}

/// @brief
/// @param req
/// @param res
/// @return
auto Express::parseUrlencoded(Request &req, Response &res) -> bool
{
    if (req.body != nullptr && req.body.length() > 0)
    {
        LOG_I(F("Body already read"));
        return true;
    }

    if (req.get(ContentType).equalsIgnoreCase(F("application/x-www-form-urlencoded")))
    {
        LOG_I(F("> bodyparser x-www-form-urlencoded"));
    }
    else
        LOG_V(F("Not an application/x-www-form-urlencoded body"));

    return true;
}

/// @brief
/// @param path
/// @param pathItems
/// @param requestPath
/// @param requestPathItems
/// @param params
/// @return
auto Express::match(const String &path, const std::vector<PosLen> &pathItems,
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
            if (requestPath.substring(ave.pos, ave.pos + ave.len) != path.substring(bve.pos, bve.pos + bve.len))
            {
                return false;
            }
        }
    }

    return true;
}

/// @brief
/// @param req
/// @param res
/// @return
auto Express::evaluate(Request &req, Response &res) -> const bool
{
    LOG_V(F("evaluate"), req.uri);

    std::vector<PosLen> req_indices{}; // TODO how many?? vis Settings

    Route::splitToVector(req.uri, req_indices);

    for (auto route : routes)
    {
        LOG_V(F("req.method:"), req.method, F("method:"), route->method);
        LOG_V(F("req.uri:"), req.uri, F("path:"), route->path);

        if ((route->method == Method::ALL || req.method == route->method) && match(route->path, route->indices, req.uri, req_indices, req.params))
        {
            res.status_ = HttpStatus::OK;
            req.route = route;

            // Route middleware
            for (const auto handler : route->middlewares)
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
    for (auto [mountPath, express] : mountPaths)
        if (express->evaluate(req, res))
            return true;

    return false;
}

/// @brief
/// @tparam ArrayType
/// @tparam ArraySize
/// @param method
/// @param path
/// @param handlers
/// @param fptrCallback
/// @return
auto Express::METHOD(const Method method, String path, const std::vector<MiddlewareCallback> middlewares, const requestCallback fptrCallback) -> Route &
{
    if (path == F("/"))
        path = F("");

    path = mountpath + path;

    LOG_I(F("METHOD:"), method, F("path:"), path, F("#middlewares:"), middlewares.size());
    // F("mountpath:"), mountpath,

    const auto route = new Route();
    route->method = method;
    route->path = path;
    route->fptrCallback = fptrCallback;

    for (auto handler : middlewares)
        if (nullptr != handler)
            route->middlewares.push_back(handler);

    route->splitToVector(route->path);
    // Add to collection
    routes.push_back(route);

    return *route;
}

/// @brief
/// @param method
/// @param path
/// @param fptr
/// @return
auto Express::METHOD(const Method method, String path, const requestCallback fptr) -> Route &
{
    LOG_I(F("METHOD:"), method, F("path:"), path);
    // F("mountpath:"), mountpath,

    const std::vector<MiddlewareCallback> middlewares = {};
    return METHOD(method, path, middlewares, fptr);
}

/// @brief
/// @param middleware
/// @return
auto Express::use(const MiddlewareCallback middleware) -> void
{
    middlewares.push_back(middleware);
}

/// @brief
/// @param middleware
/// @return
auto Express::use(const std::vector<MiddlewareCallback> middlewares) -> void
{
    for (auto middleware : middlewares)
        this->middlewares.push_back(middleware);
}

/// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
/// @param mount_path
/// @param other
/// @return
auto Express::use(const String &mount_path, Express &other) -> void
{
    LOG_I(F("use mountPath:"), mount_path);

    other.mountpath = mount_path;
    other.parent = this;
    mountPaths[other.mountpath] = &other;
}

/// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
/// @param mount_path
/// @return
auto Express::use(const String &mount_path) -> void
{
    mountpath = mount_path;
}

/// @brief
/// @param path
/// @param fptr
/// @return
auto Express::head(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::HEAD, path, fptr);
};

/// @brief
/// @param path
/// @param fptr
/// @return
auto Express::get(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::GET, path, fptr);
};

/// @brief
/// @param path
/// @param fptr
/// @return
auto Express::post(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::POST, path, fptr);
};

/// @brief
/// @param path
/// @param middleware
/// @param fptr
/// @return
auto Express::post(const String &path, const MiddlewareCallback middleware, const requestCallback fptr) -> Route &
{
    const std::vector<MiddlewareCallback> middlewares = {middleware};
    return METHOD(Method::POST, path, middlewares, fptr);
};

/// @brief
/// @param path
/// @param middleware
/// @param fptr
/// @return
auto Express::post(const String &path, const std::vector<MiddlewareCallback> middlewares, const requestCallback fptr) -> Route &
{
    return METHOD(Method::POST, path, middlewares, fptr);
};

/// @brief
/// @param path
/// @param fptr
/// @return
auto Express::put(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::PUT, path, fptr);
};

/// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
/// For more information, see the routing guide.
/// @param path
/// @param fptr
auto Express::Delete(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::DELETE, path, fptr);
}

/// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
/// @param path
/// @param fptr
auto Express::all(const String &path, const requestCallback fptr) -> Route &
{
    return METHOD(Method::ALL, path, fptr);
}

/// @brief Returns the canonical path of the app, a string.
/// @return
auto Express::path() -> String
{
    // TODO: not sure
    return (parent == nullptr) ? mountpath : parent->mountpath;
}

/// @brief Returns an instance of a single route, which you can then use to handle
/// HTTP verbs with optional middleware. Use app.route() to avoid duplicate route names
/// (and thus typo errors).
auto Express::route(const String &path) -> void
{
    // TODO
}

/// @brief
/// @param name
/// @param callback
auto Express::on(const String &name, const MountCallback callback) -> void
{
}

/// @brief
/// @param port
/// @param startedCallback
/// @return
void Express::listen(uint16_t port, const StartedCallback startedCallback)
{
    if (nullptr != server)
    {
        LOG_E(F("The listen method can only be called once! This call is ignored and processing continous."));
        return;
    }

    this->port = port;

    // Note: see https://github.com/PaulStoffregen/Ethernet/issues/42
    // change in ESP32 server.h
    // MacOS:   /Users/<user>/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
    // Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
    //      "virtual void begin(uint16_t port=0) =0;" to " virtual void begin() =0;"

    server = new EthernetServer(port);
    server->begin();

    if (startedCallback)
        startedCallback();
}

/// @brief
/// @return
auto Express::run() -> void
{
    if (auto client = server->available())
        run(client);
}

/// @brief
/// @param client
void Express::run(EthernetClient &client)
{
    while (client.connected())
    {
        if (client.available())
        {
            // Construct request object and read/parse incoming bytes
            Request req(*this, client);

            if (req.method != Method::ERROR)
            {
                Response res(*this, client);

                /// @brief run the app wide middlewares (ao bodyparsers)
                auto next = true;
                for (const auto middleware : middlewares)
                {
                    if (!middleware(req, res))
                    {
                        next = false;
                        break;
                    }
                }

                if (next)
                    evaluate(req, res);

                res.send();
            }
        }
    }
};

END_EXPRESS_NAMESPACE
