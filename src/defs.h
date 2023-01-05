#pragma once

#include <map>

#include "namespace.h"

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

BEGIN_EXPRESS_NAMESPACE

enum HttpMethod
{
    GET,     // The GET method requests a representation of the specified resource. Requests using GET should only retrieve data.
    HEAD,    // The HEAD method asks for a response identical to a GET request, but without the response body.
    POST,    // The POST method submits an entity to the specified resource, often causing a change in state or side effects on the server.
    PUT,     // The PUT method replaces all current representations of the target resource with the request payload.
    DELETE,  // The DELETE method deletes the specified resource.
    CONNECT, // The CONNECT method establishes a tunnel to the server identified by the target resource.
    OPTIONS, // The OPTIONS method describes the communication options for the target resource.
    TRACE,   // The TRACE method performs a message loop-back test along the path to the target resource.
    PATCH,   // The PATCH method applies partial modifications to a resource.
    UNDEFINED
};

struct Param
{
    String name;
    String value;
};

struct PosLen
{
    size_t pos;
    size_t len;
};

struct HttpRequest
{
    HttpMethod method;
    String version;
    String uri;
    String host;
    int contentLength;
    std::map<String, String> params;
    std::map<String, String> headers;
    std::map<String, String> arguments;
};

struct HttpResponse
{
    String body;
    uint16_t status = 404;
    std::map<String, String> headers;
};

using requestCallback = void (*)(HttpRequest &req, HttpResponse &res);

class IHttpRequestParser
{
protected:
    HttpRequest req;

public:
    virtual HttpRequest &request() { return req; }

    virtual bool parseRequest(EthernetClient &client) = 0;
};

END_EXPRESS_NAMESPACE
