#pragma once

#include <Ethernet.h>
#include <vector>

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

private:
    HttpResponse res;

    HttpResponse &evaluate(HttpRequest &req)
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
        return res;
    }

public:
    // middleware
    //    void use(String prefix) {
    //    }

    //   Express & prefix(String prefix) {
    //       _prefix = prefix;
    //
    //      return *this;
    //}

    Express &get(String uri, requestCallback fptr)
    {
        Route item{};
        item.method = HttpMethod::GET;
        item.uri = _prefix + uri;
        item.fptr = fptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        item.indices = PathCompareAndExtractParams::splitToVector(uri);

        _routes.push_back(item);

        return *this;
    };

    Express &post(String uri, requestCallback fptr)
    {
        Route item{};
        item.method = HttpMethod::POST;
        item.uri = _prefix + uri;
        item.fptr = fptr;
        // cache path splitting (avoid doing this for every request * number of paths)
        item.indices = PathCompareAndExtractParams::splitToVector(uri);

        _routes.push_back(item);

        return *this;
    };

    void run(EthernetClient &client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                if (_httpRequestParser.parseRequest(client))
                {
                    HttpRequest &req = _httpRequestParser.request();

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

                    HttpResponse &res = evaluate(req);

                    client.print("HTTP/1.1 ");
                    client.println(res.status);
                    if (!res.body.isEmpty())
                    {
                        client.print("Content-Length: ");
                        client.println(res.body.length());
                    }
                    client.println("Connection: close");
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
