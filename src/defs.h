#pragma once

#include "namespace.h"

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

BEGIN_EXPRESS_NAMESPACE

#include "httpMethods.h"
#include "httpStatusCodes.h"

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

#include "request.h"
#include "response.h"

using requestCallback = void (*)(Request &req, Response &res);
using MiddlewareCallback = bool (*)(Request &req, Response &res);
using StartedCallback = void (*)();

#include "route.h"

END_EXPRESS_NAMESPACE
