#pragma once

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

#include "utility/vector.h"
#include <map>

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

#include "httpMethods.h"
#include "httpStatusCodes.h"

// Need to move these to DefaultSettings
constexpr int maxRoutes = 10;
constexpr int maxMiddlewareCallbacks = 10;

class Request;
class Response;

constexpr size_t rawBufferSize = 512;

class Buffer
{
public:
    byte buffer[rawBufferSize]{};
    size_t byteOffset = 0;
    size_t length = 0;
};

using requestCallback = void (*)(Request &, Response &);
using MiddlewareCallback = bool (*)(Request &, Response &);
using HandlerCallback = bool (*)(Request &, Response &);
using StartedCallback = void (*)();
using DataCallback = void (*)(const Buffer &);
using EndDataCallback = void (*)();

struct Options
{
};

struct Param
{
    String name{};
    String value{};
};

struct PosLen
{
    size_t pos;
    size_t len;
};

END_EXPRESS_NAMESPACE
