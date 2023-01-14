#pragma once

#include <Ethernet.h>

#define EX_DEBUG Serial

#include "debug.h"
#include "defs.h"
#include "httpResponseParser.h"
#include "request.h"
#include "response.h"
#include "route.h"

BEGIN_EXPRESS_NAMESPACE

struct DefaultSettings
{
    /// @brief
    static constexpr int maxRoutes = 10;

    /// @brief
    static constexpr int maxMiddlewareCallbacks = 10;
};

class express
{
    friend class HttpRequestHandler;

private:
    /// @brief
    EthernetServer *server_{}; // TODO: singleton

private:
    /// @brief routes
    std::vector<Route *> routes_{};

    /// @brief Application wide middlewares
    //   MiddlewareCallback saMiddlewareCallbacks_[DefaultSettings::maxMiddlewareCallbacks];
    std::vector<MiddlewareCallback> middlewares_{};

    /// @brief
    std::map<String, express *> mount_paths_{};

    /// @brief Application Settings
    std::map<String, String> locals_{};

    /// @brief
    express *parent_ = nullptr;

private:
    // bodyparser

    // TODO: static options
    // inflate, limit, reviver, strict, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseJson(Request& req, Response& res) -> bool
    {
        EX_DBG_I(F("> bodyparser parseJson"));

        if (req.body != nullptr && req.body.length() > 0)
        {
            EX_DBG_I(F("Body already read"));
            return true;
        }

        EthernetClient &client = const_cast<EthernetClient &>(req.client_);

        if (req.get(F("content-type")).equalsIgnoreCase(F("application/json")))
        {
            auto max_length = req.get(F("content-length")).toInt();

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

            res.headers_["content-type"] = F("application/json");

            EX_DBG_I(F("< bodyparser parseJson"));

            return true;
        }

        return true;
    }

    // TODO: static options
    // inflate, limit, type, verify

    /// @brief defayults to application/octet-stream
    /// @param req
    /// @param res
    /// @return
    static auto parseRaw(Request& req, Response& res) -> bool
    {
        EX_DBG_I(F("> bodyparser raw"));

        EthernetClient &client = const_cast<EthernetClient &>(req.client_);

        if (req.get(F("content-type")).equalsIgnoreCase(F("application/octet-stream")))
        {
            auto dataLen = req.get(F("content-length")).toInt();

            while (dataLen > 0 && client.connected())
            {
                if (client.available())
                {
	                Buffer buffer;
	                buffer.length = client.read(buffer.buffer, sizeof(buffer.buffer));
                    dataLen -= buffer.length;

                    EX_DBG_I(F("remaining:"), buffer.length, dataLen);

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

        EX_DBG_I(F("< bodyparser raw"));

        return true;
    }

    // TODO: static options
    // defaultCharset, inflate, limit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseText(Request& req, Response& res) -> bool
    {
        return true;
    }

    // TODO: static options
    // extended, inflate, limit, parameterLimit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static auto parseUrlencoded(Request& req, Response& res) -> bool
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
    /// @param req
    /// @param res
    auto evaluate(Request& req, Response& res) -> bool
    {
        EX_DBG_I(F("evaluate"), req.uri_);

        std::vector<PosLen> req_indices{};

        Route::splitToVector(req.uri_, req_indices);

        for (auto route : routes_)
        {
            EX_DBG_I(F("req.method:"), req.method, F("method:"), route->method);
            EX_DBG_I(F("req.uri:"), req.uri_, F("path:"), route->path);

            if (req.method == route->method && Route::match(route->path, route->indices,
                                                            req.uri_, req_indices,
                                                            req.params))
            {
                res.status_ = HTTP_STATUS_OK; // assumes all goes OK
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
    auto METHOD(const Method method, String path, const HandlerCallback handler, const requestCallback fptrCallback) -> Route&
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
    auto METHOD(const Method method, String path, const requestCallback fptr) -> Route&
    {
        EX_DBG_I(F("METHOD:"), method, F("mountpath:"), mountpath, F("path:"), path);
        return METHOD(method, path, nullptr, fptr);
    }

public:
    /// @brief Constructor
    express()
    {
        EX_DBG_V(F("Express() constructor"));

        settings[F("env")] = F("production");
        //  settings[F("X-powered-by")] = F("X-Powered-By: Express for Arduino");
    }

    /// @brief
    uint16_t port{};

    /// @brief Application Settings
    std::map<String, String> settings{};

    /// @brief The app.mountpath property contains the path patterns
    /// on which a sub-app was mounted.
    String mountpath{};

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
    auto use(const String& mount_path, express& other) -> void
    {
        EX_DBG_I(F("use mountPath:"), mount_path);

        other.mountpath = mount_path;
        other.parent_ = this;
        mount_paths_[other.mountpath] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    auto use(const String& mount_path) -> void
    {
        mountpath = mount_path;
    }

    /// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
    /// @param path
    /// @param fptr
    auto all(const String& path, const requestCallback fptr) -> void
    {
        // TODO: not implemented
    }

    /// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
    /// For more information, see the routing guide.
    /// @param path
    /// @param fptr
    auto Delete(const String& path, const requestCallback fptr) -> void
    {
        METHOD(Method::DELETE, path, fptr);
    }

    /// @brief Sets the Boolean setting name to false, where name is one of the properties from
    /// the app settings table. Calling app.set('foo', false) for a Boolean property is the
    /// same as calling app.disable('foo').
    /// @param name
    auto disable(const String& name) -> void
    {
        settings[name] = "false";
    }

    /// @brief Returns true if the Boolean setting name is disabled (false), where name is one
    /// of the properties from the app settings table.
    /// @param name
    /// @return
    auto disabled(const String& name) -> String
    {
        return settings[name];
    }

    /// @brief Sets the Boolean setting name to true, where name is one of the properties from the
    /// app settings table. Calling app.set('foo', true) for a Boolean property is the same as
    /// calling app.enable('foo').
    /// @param name
    auto enable(const String& name) -> void
    {
        settings[name] = "true";
    }

    /// @brief Returns true if the setting name is enabled (true), where name is one of the
    /// properties from the app settings table.
    /// @param name
    /// @return
    auto enabled(const String& name) -> String
    {
        return settings[name];
    }

    /// @brief Returns the value of name app setting, where name is one of the strings in
    /// the app settings table. For example:
    /// @param name
    /// @return
    auto get(const String& name) -> String
    {
        // TODO
        return F("");
    }

    /// @brief Assigns setting name to value. You may store any value that you want, but
    // certain names can be used to configure the behavior of the server. These special
    /// names are listed in the app settings table.
    /// @param name
    /// @param value
    auto set(const String& name, const String& value) -> void
    {
        // TODO
    }

#pragma region HTTP_Methods

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto get(const String& path, const requestCallback fptr) -> Route&
    {
        return METHOD(Method::GET, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto post(const String& path, const requestCallback fptr) -> Route&
    {
        return METHOD(Method::POST, path, fptr);
    };

    /// @brief
    /// @param path
    /// @param middleware
    /// @param fptr
    /// @return
    auto post(const String& path, const MiddlewareCallback middleware, const requestCallback fptr) -> Route&
    {
        return METHOD(Method::POST, path, middleware, fptr);
    };

    /// @brief
    /// @param path
    /// @param fptr
    /// @return
    auto put(const String& path, const requestCallback fptr) -> Route&
    {
        return METHOD(Method::PUT, path, fptr);
    };

#pragma endregion HTTP_Methods

    /// @brief Returns the canonical path of the app, a string.
    /// @return
    auto path() -> String
    {
        // TODO: noy sure
        return (parent_ == nullptr) ? mountpath : parent_->mountpath;
    }

    /// @brief Returns an instance of a single route, which you can then use to handle
    /// HTTP verbs with optional middleware. Use app.route() to avoid duplicate route names
    /// (and thus typo errors).
    auto route(const String& path) -> void
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
        if (EthernetClient client = server_->available())
            run(client);
    }

    /// @brief
    /// @param client
    auto run(EthernetClient& client) -> void
    {
        while (client.connected())
        {
            if (client.available())
            {
                Request req(*this, client);

                HttpRequestParser http_request_parser_;
                http_request_parser_.parseRequest(client, req);

                if (req.method != Method::ERROR)
                {
                    Response res(*this);

                    /// @brief run the app wide middlewares (ao bodyparsers)
                    for (const auto middleware : middlewares_)
                        if (!middleware(req, res))
                            break;

                    /// @brief evaluate the request
                    evaluate(req, res);

                    client.print(F("HTTP/1.1 "));
                    client.println(res.status_);

                    // Add to headers
                    if (!res.body_.isEmpty())
                        res.headers_[F("content-length")] = res.body_.length();
                    res.headers_[F("connection")] = F("close");
                    if (settings[F("X-powered-by")])
                        res.headers_[F("X-powered-by")] = settings[F("X-powered-by")];

                    // Send headers
                    for (auto [first, second] : res.headers_)
                    {
                        client.print(first);
                        client.print(": ");
                        client.println(second);
                    }
                    // headers are done
                    client.println();

                    // send content length *or* close the connection (spec 7.2.2)
                    if (res.body_ && !res.body_.isEmpty())
                    {
                        client.println(res.body_.c_str());
                    }

                    client.stop();
                }
            }
        }
    };
};

END_EXPRESS_NAMESPACE

#include "Express.hpp"
