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

        auto &client = const_cast<EthernetClient &>(req.client_);

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

        auto &client = const_cast<EthernetClient &>(req.client_);

        while (dataLen > 0 && client.connected())
        {
            if (client.available())
            {
                Buffer buffer;
                buffer.length = client.read(buffer.buffer, sizeof(buffer.buffer));
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
/// @param req
/// @param res
/// @return
auto Express::evaluate(Request &req, Response &res) -> const bool
{
    LOG_V(F("evaluate"), req.uri_);

    std::vector<PosLen> req_indices{}; // TODO how many?? vis Settings

    Route ::splitToVector(req.uri_, req_indices);

    for (auto route : routes_)
    {
        LOG_V(F("req.method:"), req.method, F("method:"), route->method);
        LOG_V(F("req.uri:"), req.uri_, F("path:"), route->path);

        if ((route->method == Method::ALL || req.method == route->method) && match(route->path, route->indices, req.uri_, req_indices, req.params))
        {
            res.status_ = HttpStatus::OK;
            req.route = route;

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
    for (auto [mountPath, express] : mountPaths_)
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
template <typename ArrayType, size_t ArraySize>
auto Express::METHOD(const Method method, String path, ArrayType (&handlers)[ArraySize], const requestCallback fptrCallback) -> Route &
{
    if (path == F("/"))
        path = F("");

    path = mountpath + path;

    LOG_I(F("METHOD:"), method, F("path:"), path, F("#handlers:"), ArraySize);
    // F("mountpath:"), mountpath,

    const auto route = new Route();
    route->method = method;
    route->path = path;
    route->fptrCallback = fptrCallback;

    for (auto handler : handlers)
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
auto Express::METHOD(const Method method, String path, const requestCallback fptr) -> Route &
{
    LOG_I(F("METHOD:"), method, F("path:"), path);
    // F("mountpath:"), mountpath,

    const MiddlewareCallback middlewares[] = {0};
    return METHOD(method, path, middlewares, fptr);
}

/// @brief
/// @param port
/// @param startedCallback
/// @return
void Express::listen(uint16_t port, const StartedCallback startedCallback)
{
    if (nullptr != server_)
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

    server_ = new EthernetServer(port);
    server_->begin();

    if (startedCallback)
        startedCallback();
}

/// @brief
/// @return
auto Express::run() -> void
{
    if (auto client = server_->available())
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
            Request req(this, client);

            if (req.method != Method::ERROR)
            {
                Response res(this, client);

                /// @brief run the app wide middlewares (ao bodyparsers)
                auto next = true;
                for (const auto middleware : middlewares_) {
                    if (!middleware(req, res)) {
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
