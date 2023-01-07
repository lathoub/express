#pragma once

#include <Ethernet.h>
#include <vector>
#include <list>

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
        HttpMethod method = HttpMethod::UNDEFINED;
        String uri;
        requestCallback fptr = nullptr;
        std::vector<PosLen> indices;
    };

    std::vector<Route> _routes;

    String _prefix;

    friend class HttpRequestHandler;
    HttpRequestParser _httpRequestParser;

    std::list<MiddlewareCallback> _middlewares;
    std::list<MiddlewareCallback>::iterator _mwi;

private:
    void evaluate(HttpRequest &req, HttpResponse &res)
    {
        res.body = "";
        res.status = 404;
        res.headers.clear();
        auto req_indices = PathCompareAndExtractParams::splitToVector(req.uri);

        for (auto [method, uri, fptr, indices] : _routes)
        {
            if (req.method == method && PathCompareAndExtractParams::match(
                                            uri, indices,
                                            req.uri, req_indices,
                                            req.params))
            {
                res.status = 0;
                fptr(req, res);
                break;
            }
        }
    }

public:
    // Add middleware
    Express &use(MiddlewareCallback middleware)
    {
        _middlewares.push_back(middleware);
        return *this;
    }

    Express &use(String prefix)
    {
        _prefix = prefix;
        return *this;
    }

    Express &get(String uri, requestCallback fptr)
    {
        if (uri == "/")
            uri = "";

        Route item{};
        item.method = HttpMethod::GET;
        item.uri = _prefix + uri;
        item.fptr = fptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        item.indices = PathCompareAndExtractParams::splitToVector(item.uri);

        _routes.push_back(item);

        return *this;
    };

    Express &post(String uri, requestCallback fptr)
    {
        if (uri == "/")
            uri = "";

        Route item{};
        item.method = HttpMethod::POST;
        item.uri = _prefix + uri;
        item.fptr = fptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        item.indices = PathCompareAndExtractParams::splitToVector(item.uri);

        _routes.push_back(item);

        return *this;
    };

    void run(EthernetClient &client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                HttpRequest &req = _httpRequestParser.parseRequest(client);
                HttpResponse res;

                if (req.method != HttpMethod::ERROR)
                {
                    _mwi = _middlewares.begin();
                    while (_mwi != _middlewares.end())
                    {
                        if ((*_mwi)(req, res))
                            _mwi++;
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
                    client.println(res.status);
                    for (auto [first, second] : res.headers)
                    {
                        client.print(first);
                        client.print(": ");
                        client.println(second);
                    }
                    if (!res.body.isEmpty())
                    {
                        client.print("content-length: ");
                        client.println(res.body.length());
                    }
                    client.println("connection: close");
                    client.println("");
                    // send content length *or* close the connection (spec 7.2.2)
                    if (!res.body.isEmpty())
                        client.println(res.body.c_str());
                    client.stop();
                }
            }
        }
    };
};

END_EXPRESS_NAMESPACE

#include "Express.hpp"
