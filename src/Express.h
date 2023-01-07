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
    struct Route
    {
        Method method = Method::UNDEFINED;
        String uri{};
        requestCallback fptr = nullptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        std::vector<PosLen> indices{};
    };

    std::vector<Route> routes_{};

    friend class HttpRequestHandler;
    HttpRequestParser http_request_parser_;

    std::list<MiddlewareCallback> middlewares_{};

    EthernetServer *server_{}; // TODO: singleton

    String mount_path_{};
    
    std::map<String, Express*> mount_paths_{};

private:
    /// @brief
    /// @param req
    /// @param res
    bool evaluate(Request &req, Response &res)
    {
        res.body_ = "";
        res.status_ = 404;
        res.headers_.clear();
        const auto req_indices = PathCompareAndExtractParams::splitToVector(req.uri);

        for (auto [method, uri, fptr, indices] : routes_)
        {
            if (req.method == method && PathCompareAndExtractParams::match(
                                            uri, indices,
                                            req.uri, req_indices,
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

public:
    uint16_t port{};

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
        other.mount_path_ = mount_path;
        mount_paths_[other.mount_path_] = &other;
    }

    /// @brief The app.mountpath property contains one or more path patterns on which a sub-app was mounted.
    /// @param mount_path
    /// @return
    void use(String mount_path)
    {
        mount_path_ = mount_path;
    }

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void get(String uri, const requestCallback fptr)
    {
        if (uri == "/")
            uri = "";

        uri = mount_path_ + uri;

        Route item{};
        item.method = Method::GET;
        item.uri = uri;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(item.uri);

        routes_.push_back(item);
    };

    /// @brief
    /// @param uri
    /// @param fptr
    /// @return
    void post(String uri, const requestCallback fptr)
    {
        if (uri == "/")
            uri = "";

        uri = mount_path_ + uri;

        Route item{};
        item.method = Method::POST;
        item.uri = uri;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(item.uri);

        routes_.push_back(item);
    };

    /// @brief
    void listen(uint16_t port, const StartedCallback fptr = nullptr)
    {
        this->port = port;

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

                if (req.method != Method::ERROR)
                {
	                Response res;

	                auto it = middlewares_.begin();
                    while (it != middlewares_.end())
                    {
                        if ((*it)(req, res))
                            ++it;
                        else
                            break;
                    }

                    /*
                                        EX_DBG("method", req.method);
                                        EX_DBG("uri", req.uri);
                                        EX_DBG("version", req.version);
                                        EX_DBG("host", req.host);
                                        EX_DBG(F("headers:"));
                                        for (auto [first, second] : req.headers)
                                        {
                                            EX_DBG(F("key:"), first, F("value:"), second);
                                        }

                                        EX_DBG(F("arguments:"));
                                        for (auto [first, second] : req.arguments)
                                        {
                                            EX_DBG(F("key:"), first, F("value:"), second);
                                        }
                    */
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
