#pragma once

#include <Ethernet.h>
#include <vector>

//#define DEBUG Serial

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

    std::vector<Route> routes;

    friend class HttpRequestHandler;
    HttpRequestParser httpRequestParser;

private:
    HttpResponse res;

    HttpResponse &evaluate(HttpRequest &req)
    {
        res.status = 404;

        for (auto [method, uri, fptr, indices] : routes)
        {
            if (req.method == method && PathCompareAndExtractParams::match(
                uri, indices,
                req.uri, PathCompareAndExtractParams::splitToVector(req.uri), 
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
    void get(String uri, requestCallback fptr)
    {
        Route item{};
        item.method = HttpMethod::GET;
        item.uri = uri;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(uri);

        routes.push_back(item);
    };

    void post(String uri, requestCallback fptr)
    {
        Route item{};
        item.method = HttpMethod::POST;
        item.uri = uri;
        item.fptr = fptr;
        item.indices = PathCompareAndExtractParams::splitToVector(uri);

        routes.push_back(item);
    };

    void run(EthernetClient &client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                if (httpRequestParser.parseRequest(client))
                {
                    HttpRequest &req = httpRequestParser.request();

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
