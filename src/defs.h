/*!
 *  @file       defs.h
 *  Project     Arduino Express Library
 *  @brief      Fast, unopinionated, (very) minimalist web framework for Arduino
 *  @author     lathoub
 *  @date       20/01/23
 *  @license    GNU GENERAL PUBLIC LICENSE
 *
 *   Fast, unopinionated, (very) minimalist web framework for Arduino.
 *   Copyright (C) 2023 lathoub
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

#if PLATFORM == ESP32_W5500
#include <Ethernet.h>
// Note: see https://github.com/PaulStoffregen/Ethernet/issues/42
// change in ESP32 server.h
// MacOS:
// /Users/<user>/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
// Windows:
// C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
//      "virtual void begin(uint16_t port=0) =0;" to " virtual void begin()
//      =0;"
#define ServerType EthernetServer
#define ClientType EthernetClient
#endif

// TODO: using templates
#if PLATFORM == ESP32
#include <WiFi.h>
#define ServerType WiFiServer
#define ClientType WiFiClient
#endif

#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include "utility/logger.h"

#ifdef ESP32
#define USE_STDCONTAINERS
#endif

#ifdef USE_STDCONTAINERS
#include <map>
#include <vector>
#else
#error "Alternative for std::vector and std::map here"
#endif

typedef std::map<String, String> locals_t;
typedef std::map<String, String> params_t;

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

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

const String __dirname{};

using ContentCallback = const char *(*)();
using WriteCallback = void (*)(const char *, int);

struct File {
  String filename;
  ContentCallback contentsCallback;
  size_t length() { return strlen(contentsCallback()); } 
};

#include "Buffer.hpp"

/// @brief When std::vector's are not available, an
/// alternative implementation uses fixed length containers.
/// The max len is set here - override if needed
struct DefaultSettings {};

struct beginEnd {
  int start;
  int end;
};

struct Range {
  String type;
  std::vector<beginEnd> ranges;
};

class Options {
public:
  /// Object containing HTTP headers to serve with the file.
  std::map<String, String> headers{};
  /// Enable or disable accepting ranged requests.
  bool acceptRanges = true;
  /// Enable or disable setting Cache-Control response header
  bool cacheControl = true;
  /// Enable or disable the immutable directive in the Cache-Control response
  /// header. If enabled, the maxAge option should also be specified to enable
  /// caching. The immutable directive will prevent supported clients from
  /// making conditional requests during the life of the maxAge option to check
  /// if the file has changed.
  bool immutable = false;
  /// Option for serving dotfiles. Possible values are “allow”, “deny”,
  /// “ignore”.
  String dotfiles = F("ignore");
  /// Sets the max-age property of the Cache-Control header in milliseconds or a
  /// string in ms format
  int maxAge = 0;
  /// Sets the max-age property of the Cache-Control header in milliseconds or a
  /// string in ms format
  String root{};

  /// @brief Default constructor
  Options() {}

  /// @brief Copy constructor
  Options(Options *another) {
    this->headers = another->headers;
    this->acceptRanges = another->acceptRanges;
    this->cacheControl = another->cacheControl;
    this->immutable = another->immutable;
    this->dotfiles = another->dotfiles;
    this->maxAge = another->maxAge;
    this->root = another->root;
  }
};

struct PosLen {
  size_t pos;
  size_t len;
};

enum Method {
  GET,    // The GET method requests a representation of the specified resource.
          // Requests using GET should only retrieve data.
  HEAD,   // The HEAD method asks for a response identical to a GET request, but
          // without the response body.
  POST,   // The POST method submits an entity to the specified resource, often
          // causing a change in state or side effects on the server.
  PUT,    // The PUT method replaces all current representations of the target
          // resource with the request payload.
  DELETE, // The DELETE method deletes the specified resource.
  CONNECT, // The CONNECT method establishes a tunnel to the server identified
           // by the target resource.
  OPTIONS, // The OPTIONS method describes the communication options for the
           // target resource.
  TRACE, // The TRACE method performs a message loop-back test along the path to
         // the target resource.
  PATCH, // The PATCH method applies partial modifications to a resource.
  ALL,
  UNDEFINED = 999,
  ERROR = 999,
};

enum HttpStatus {
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
  RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,
  I_AM_A_TEAPOT = 418,
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
