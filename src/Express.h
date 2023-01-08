#pragma once

#include <Ethernet.h>
#include <vector>
#include <list>
#include <map>

#define DEBUG Serial

#include "debug.h"
#include "defs.h"
#include "httpResponseParser.h"
#include "pathCompare.h"

BEGIN_EXPRESS_NAMESPACE

class Express
{
    friend class HttpRequestHandler;

private:
    std::vector<Route> routes_{};

    std::list<MiddlewareCallback> middlewares_{};

    EthernetServer *server_{}; // TODO: singleton

    std::map<String, Express *> mount_paths_{};

    /// @brief Application Settings
    std::map<String, String> locals{};

    /// @brief
    Express *parent_ = nullptr;

private:
    /// @brief
    /// @param req
    /// @param res
    bool evaluate(Request &req, Response &res)
    {
        res.body_ = "";
        res.status_ = 404;
        res.headers_.clear();
        const auto req_indices = PathCompareAndExtractParams::splitToVector(req.uri_);

        for (auto [method, uri, fptr, indices] : routes_)
        {
            EX_DBG(F("---------------------------------------------"));
            EX_DBG(F("req.method:"), req.method, F("method:"), method);
            EX_DBG(F("req.uri:"), req.uri_, F("uri:"), uri);

            if (req.method == method && PathCompareAndExtractParams::match(
                                            uri, indices,
                                            req.uri_, req_indices,
                                            req.params))
            {
                EX_DBG(F("Match"));

                res.status_ = HTTP_STATUS_OK;
                fptr(req, res);
                return true;
            }
            else
            {
                EX_DBG(F("No match"));
            }
        }

        for (auto [mountPath, express] : mount_paths_)
        {
            if (express->evaluate(req, res))
                return true;
        }

        return false;
    }

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void METHOD(Method method, String path, const requestCallback fptr)
    {
        //        if (path == F("/"))
        //          path = F("");

        path = mountpath + path;

        Route item{};
        item.method = method;
        item.path = path;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(item.path);

        routes_.push_back(item);
    }

public:
    /// @brief Constructor
    Express()
    {
        settings[F("env")] = F("production");
        settings[F("X-powered-by")] = F("X-Powered-By: Express for Arduino");
    }

    /// @brief
    uint16_t port{};

    /// @brief Application Settings
    std::map<String, String> settings{};

    /// @brief The app.mountpath property contains the path patterns
    /// on which a sub-app was mounted.
    String mountpath{};

    /// Methods

    /// @brief
    /// @param middleware
    /// @return
    void use(const MiddlewareCallback middleware)
    {
        middlewares_.push_back(middleware);
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @param other
    /// @return
    void use(const String &mount_path, Express &other)
    {
        other.mountpath = mount_path;
        other.parent_ = this;
        mount_paths_[other.mountpath] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    void use(const String &mount_path)
    {
        mountpath = mount_path;
    }

    /// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
    /// @param uri
    /// @param fptr
    void all(const String &path, const requestCallback fptr)
    {
        // TODO: not implemented
    }

    /// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
    /// For more information, see the routing guide.
    /// @param uri
    /// @param fptr
    void Delete(const String &path, const requestCallback fptr)
    {
        METHOD(Method::DELETE, path, fptr);
    }

    /// @brief Sets the Boolean setting name to false, where name is one of the properties from
    /// the app settings table. Calling app.set('foo', false) for a Boolean property is the
    /// same as calling app.disable('foo').
    /// @param uri
    void disable(const String &name)
    {
        settings[name] = "false";
    }

    /// @brief Returns true if the Boolean setting name is disabled (false), where name is one
    /// of the properties from the app settings table.
    /// @param name
    /// @return
    String disabled(const String &name)
    {
        return settings[name];
    }

    /// @brief Sets the Boolean setting name to true, where name is one of the properties from the
    /// app settings table. Calling app.set('foo', true) for a Boolean property is the same as
    /// calling app.enable('foo').
    /// @param name
    void enable(const String &name)
    {
        settings[name] = "true";
    }

    /// @brief Returns true if the setting name is enabled (true), where name is one of the
    /// properties from the app settings table.
    /// @param name
    /// @return
    String enabled(const String &name)
    {
        return settings[name];
    }

    /// @brief Returns the value of name app setting, where name is one of the strings in
    /// the app settings table. For example:
    /// @param name
    /// @return
    String get(const String &name)
    {
        // TODO
        return F("");
    }

    /// @brief Assigns setting name to value. You may store any value that you want, but
    // certain names can be used to configure the behavior of the server. These special
    /// names are listed in the app settings table.
    /// @param name
    /// @param value
    void set(const String &name, const String &value)
    {
        // TODO
    }

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void get(const String &path, const requestCallback fptr)
    {
        METHOD(Method::GET, path, fptr);
    };

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void post(const String &path, const requestCallback fptr)
    {
        METHOD(Method::POST, path, fptr);
    };

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void put(const String &path, const requestCallback fptr)
    {
        METHOD(Method::PUT, path, fptr);
    };

    /// @brief Returns the canonical path of the app, a string.
    /// @return
    String path()
    {
        // TODO: noy sure
        return (parent_ == nullptr) ? mountpath : parent_->mountpath;
    }

    /// @brief Returns an instance of a single route, which you can then use to handle
    /// HTTP verbs with optional middleware. Use app.route() to avoid duplicate route names
    /// (and thus typo errors).
    void route(const String &path)
    {
        // TODO
    }

    /// @brief
    void listen(uint16_t port, const StartedCallback fptr = nullptr)
    {
        this->port = port;

        // Note: see https://github.com/PaulStoffregen/Ethernet/issues/42
        // change in ESP32 server.h
        // MacOS:   /Users/<user>/Library/Arduino15/packages/esp32/hardware/esp32/2.0.5/cores/esp32
        // Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.5\cores\esp32\Server.h
        //      "virtual void begin(uint16_t port=0) =0;" to " virtual void begin() =0;"

        server_ = new EthernetServer(port);
        server_->begin();

        if (fptr)
            fptr();
    }

    /// @brief
    void run()
    {
        if (EthernetClient client = server_->available())
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
                HttpRequestParser http_request_parser_;
                Request &req = http_request_parser_.parseRequest(client);
                req.app = this;

                if (req.method != Method::ERROR)
                {
                    Response res;
                    res.app = this;

                    auto it = middlewares_.begin();
                    while (it != middlewares_.end())
                    {
                        if ((*it)(req, res))
                            ++it;
                        else
                            break;
                    }

                    if (!(it != middlewares_.end()))
                        evaluate(req, res);

                    client.print(F("HTTP/1.1 "));
                    client.println(res.status_);
                    for (auto [first, second] : res.headers_)
                    {
                        client.print(first);
                        client.print(": ");
                        client.println(second);
                    }
                    if (!res.body_.isEmpty())
                    {
                        client.print(F("content-length: "));
                        client.println(res.body_.length());
                    }
                    client.println(F("connection: close"));
                    client.println(F(""));
                    // send content length *or* close the connection (spec 7.2.2)
                    if (!res.body_.isEmpty())
                        client.println(res.body_.c_str());
                    client.stop();
                }
            }
        }
    };
};

END_EXPRESS_NAMESPACE

#include "Express.hpp"
