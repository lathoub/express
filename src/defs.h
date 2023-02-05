#pragma once

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

#include "utility/vector.h"
#include "utility/dictionary.h"
typedef dictionary<String, String, 50> locals_t;
typedef dictionary<String, String> params_t;

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

constexpr size_t rawBufferSize = 512;

#define EXPRESS_LIBRARY_VERSION 0x000100
#define EXPRESS_LIBRARY_VERSION_MAJOR 0
#define EXPRESS_LIBRARY_VERSION_MINOR 1
#define EXPRESS_LIBRARY_VERSION_PATCH 0

// cache Strings that are used a lot, to preserve memory
static const char ContentType[] PROGMEM = "content-type";
static const char ContentLength[] PROGMEM = "content-length";
static const char XPoweredBy[] PROGMEM = "x-powered-by";
static const char ApplicationJson[] PROGMEM = "application/json";
static const char True[] PROGMEM = "true";
static const char False[] PROGMEM = "false";

struct DefaultSettings
{
    /// @brief
    static constexpr int MaxRoutes = 15;

    /// @brief
    static constexpr int MaxMountPaths = 5;

    /// @brief
    static constexpr int MaxMiddlewareCallbacks = 5;

    /// @brief
    static constexpr int MaxSettings = 4;

    /// @brief
    static constexpr int MaxEngines = 4;

    /// @brief 
    static constexpr int MaxHeaders = 5;

    /// @brief
    static constexpr int MaxQueries = 10;
};

class Buffer
{
public:
    byte buffer[rawBufferSize]{};
    size_t byteOffset = 0;
    size_t length = 0;
};

class Options
{
public:
    dictionary<String, String, 10> headers{};
};

struct PosLen
{
    size_t pos;
    size_t len;
};

enum Method
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
    UNDEFINED = 999,
    ERROR = 999,
};

enum HttpStatus
{
    CONTINUE = 100,
    SWITCH_PROTOCOLS = 101,
    PROCESSING = 102,
    EARLYHINTS = 103,

    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    PARTIAL = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    ALREADY_REPORTED = 208,

    AMBIGUOUS = 300,
    MOVED = 301,
    REDIRECT = 302,
    REDIRECT_METHOD = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    REDIRECT_KEEP_VERB = 307,

    BAD_REQUEST = 400,
    DENIED = 401,
    PAYMENT_REQ = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    BAD_METHOD = 405,
    NONE_ACCEPTABLE = 406,
    PROXY_AUTH_REQ = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECOND_FAILED = 412,
    REQUEST_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA = 415,
    RETRY_WITH = 449,

    SERVER_ERROR = 500,
    NOT_SUPPORTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAIL = 503,
    GATEWAY_TIMEOUT = 504,
    VERSION_NOT_SUP = 505,
    VARIANT_ALSO_NEGOTIATES = 506,
    INSUFFICIANT_STORAGE = 507,
    LOOP_DETECTED = 508,
    NOT_EXTENDED = 510,
};

END_EXPRESS_NAMESPACE
