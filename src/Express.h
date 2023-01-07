#pragma once

#include <Ethernet.h>
#include <vector>
#include <list>
#include <map>

// #define DEBUG Serial

#include "debug.h"
#include "defs.h"
#include "httpResponseParser.h"
#include "pathCompare.h"

BEGIN_EXPRESS_NAMESPACE

class Express
{
private:
    class Route
    {
    private:
    public:
        Method method = Method::UNDEFINED;
        String path{};
        requestCallback fptr = nullptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        std::vector<PosLen> indices{};
    };

    std::vector<Route> routes_{};

    friend class HttpRequestHandler;
    HttpRequestParser http_request_parser_;

    std::list<MiddlewareCallback> middlewares_{};

    EthernetServer *server_{}; // TODO: singleton

    std::map<String, Express *> mount_paths_{};

    /// @brief Application Settings
    std::map<String, bool> settings_{};

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
            if (req.method == method && PathCompareAndExtractParams::match(
                                            uri, indices,
                                            req.uri_, req_indices,
                                            req.params))
            {
                res.status_ = 0;
                fptr(req, res);
                return true;
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
        if (path == "/")
            path = "";

        path = mountpath + path;

        Route item{};
        item.method = method;
        item.path = path;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(item.path);

        routes_.push_back(item);
    }

public:
    uint16_t port{};

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
    void use(String mount_path, Express &other)
    {
        other.mountpath = mount_path;
        other.parent_ = this;
        mount_paths_[other.mountpath] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    void use(String mount_path)
    {
        mountpath = mount_path;
    }

    /// @brief This method is like the standard app.METHOD() methods, except it matches all HTTP verbs.
    /// @param uri
    /// @param fptr
    void all(String path, const requestCallback fptr)
    {
        // TODO: not implemented
    }

    /// @brief Routes HTTP DELETE requests to the specified path with the specified callback functions.
    /// For more information, see the routing guide.
    /// @param uri
    /// @param fptr
    void Delete(String path, const requestCallback fptr)
    {
        METHOD(Method::DELETE, path, fptr);
    }

    /// @brief Sets the Boolean setting name to false, where name is one of the properties from
    /// the app settings table. Calling app.set('foo', false) for a Boolean property is the
    /// same as calling app.disable('foo').
    /// @param uri
    void disable(String name)
    {
        settings_[name] = false;
    }

    /// @brief Returns true if the Boolean setting name is disabled (false), where name is one
    /// of the properties from the app settings table.
    /// @param name
    /// @return
    bool disabled(String name)
    {
        return settings_[name];
    }

    /// @brief Sets the Boolean setting name to true, where name is one of the properties from the
    /// app settings table. Calling app.set('foo', true) for a Boolean property is the same as
    /// calling app.enable('foo').
    /// @param name
    void enable(String name)
    {
        settings_[name] = true;
    }

    /// @brief Returns true if the setting name is enabled (true), where name is one of the
    /// properties from the app settings table.
    /// @param name
    /// @return
    bool enabled(String name)
    {
        return settings_[name];
    }

    /// @brief Returns the value of name app setting, where name is one of the strings in
    /// the app settings table. For example:
    /// @param name
    /// @return
    String get(String name)
    {
        // TODO
        return "";
    }

    /// @brief Assigns setting name to value. You may store any value that you want, but
    // certain names can be used to configure the behavior of the server. These special
    /// names are listed in the app settings table.
    /// @param name
    /// @param value
    void set(String name, String value)
    {
        // TODO
    }

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void get(String path, const requestCallback fptr)
    {
        METHOD(Method::GET, path, fptr);
    };

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void post(String path, const requestCallback fptr)
    {
        METHOD(Method::POST, path, fptr);
    };

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void put(String path, const requestCallback fptr)
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
    void route(String path)
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

                    client.print("HTTP/1.1 ");
                    client.println(res.status_);
                    for (auto [first, second] : res.headers_)
                    {
                        client.print(first);
                        client.print(": ");
                        client.println(second);
                    }
                    if (!res.body_.isEmpty())
                    {
                        client.print("content-length: ");
                        client.println(res.body_.length());
                    }
                    client.println("connection: close");
                    client.println("");
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
