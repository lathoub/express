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
const int maxRoutes = 10;
const int maxMiddlewareCallbacks = 10;

class Request;
class Response;

using requestCallback = void (*)(Request &req, Response &res);
using MiddlewareCallback = bool (*)(Request &req, Response &res);
using StartedCallback = void (*)();
using DataCallback = void (*)(int);
using EndDataCallback = void (*)();

struct Options
{
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

END_EXPRESS_NAMESPACE
