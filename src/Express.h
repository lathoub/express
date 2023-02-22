/*!
 *  @file       _Express.h
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

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

// Forward declaration
class _Request;
class _Response;
class _Route;
class _Error;
class _Router;
class _Express;

// Callback definitions
using NextCallback = void (*)(const _Error *error);
using ErrorCallback = void (*)(_Error &, _Request &, _Response &,
                               const NextCallback next);
using MiddlewareCallback = void (*)(_Request &, _Response &,
                                    const NextCallback next);
using RenderEngineCallback = void (*)(ClientType &, locals_t &locals, Options *,
                                      const char *f);
using Callback = void (*)();
using DataCallback = void (*)(const Buffer &);
using EndDataCallback = void (*)();
using MountCallback = void (*)(_Express *);
using Write_Callback = void (*)(const char *, const uint &);

/// @brief
class _Error {
public:
  uint32_t status;
  String message;
  _Error(const uint32_t &, const String & msg = F(""));
};

/// @brief
class _Express {
private:
  /// @brief
  ServerType *server{};

  /// @brief
  _Router *router_;

public:
  /// @brief Constructor
  _Express();

  /// @brief
  uint16_t port{};

  /// @brief Application Settings
  std::map<String, String> settings;

  /// @brief The app.mountpath property contains the path patterns
  /// on which a sub-app was mounted.
  String mountpath{};

  /// @brief
  std::map<String, RenderEngineCallback> engines;

  /// @brief The app.locals object has properties that are local variables
  /// within the application, and will be available in templates rendered
  // with res.render.
  /// Once set, the value of app.locals properties persist throughout the
  // life of the application, in contrast with res.locals properties that
  /// are valid only for the lifetime of the request.
  locals_t locals;

#pragma region _Express()

private:
  // bodyparser

  // TODO: static options
  // inflate, limit, reviver, strict, type, verify

  /// @brief
  /// @param req
  /// @param res
  /// @return
  static auto parseJson(_Request &, _Response &,
                        const NextCallback callback = nullptr) -> void;

  // TODO: static options
  // inflate, limit, type, verify

  /// @brief
  /// @param req
  /// @param res
  /// @return
  static auto parseRaw(_Request &, _Response &,
                       const NextCallback callback = nullptr) -> void;

  // TODO: static options
  // defaultCharset, inflate, limit, type, verify

  /// @brief
  /// @param req
  /// @param res
  /// @return
  static auto parseText(_Request &, _Response &,
                        const NextCallback callback = nullptr) -> void;

  // TODO: static options
  // extended, inflate, limit, parameterLimit, type, verify

  /// @brief
  /// @param req
  /// @param res
  /// @return
  static auto parseUrlencoded(_Request &, _Response &,
                              const NextCallback callback = nullptr) -> void;

  /// @brief This is a built-in middleware function in _Express. It serves
  /// static files and is based on serve-static.
  // static void Static() {}

public:
  /// @brief
  /// @return
  static auto raw() -> MiddlewareCallback;

  /// @brief
  /// @return
  static auto json() -> MiddlewareCallback;

  /// @brief
  /// @return
  static auto text() -> MiddlewareCallback;

  /// @brief This is a built-in middleware function in _Express. It parses
  /// incoming requests with urlencoded payloads and is based on body-parser.
  ///
  /// @return Returns middleware that only parses urlencoded bodies and only
  /// looks at requests where the Content-Type header matches the type option.
  /// This parser accepts only UTF-8 encoding of the body and supports automatic
  /// inflation of gzip and deflate encodings.
  static auto urlencoded() -> MiddlewareCallback;

  ///
  static auto Router() -> _Router &;

#pragma endregion _Express

private:
public:
  void param(){/* NOT IMPLEMENTED */};

  /// @brief
  /// @param middleware
  /// @return
  auto use(const ErrorCallback errorCallback) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const std::vector<ErrorCallback>) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const MiddlewareCallback middlewareCallback) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const std::vector<MiddlewareCallback>) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const String &path, const MiddlewareCallback) -> void;

  /// @brief The app.mountpath property contains one or more path patterns on
  /// which a sub-app was mounted.
  /// @param mount_path
  /// @param other
  /// @return
  auto use(const String &mount_path, _Router &other) -> void;

  /// @brief The app.mountpath property contains one or more path patterns on
  /// which a sub-app was mounted.
  /// @param mount_path
  /// @return
  auto use(const String &mount_path) -> void;

  /// @brief Sets the Boolean setting name to false, where name is one of the
  /// properties from the app settings table. Calling app.set('foo', false) for
  /// a Boolean property is the same as calling app.disable('foo').
  /// @param name
  auto disable(const String &name) -> void { settings[name] = False; }

  /// @brief Returns true if the Boolean setting name is disabled (false), where
  /// name is one of the properties from the app settings table.
  /// @param name
  /// @return
  auto disabled(const String &name) -> bool {
    return settings[name].equalsIgnoreCase(False);
  }

  /// @brief Sets the Boolean setting name to true, where name is one of the
  /// properties from the app settings table. Calling app.set('foo', true) for a
  /// Boolean property is the same as calling app.enable('foo').
  /// @param name
  auto enable(const String &name) -> void { settings[name] = True; }

  /// @brief Returns true if the setting name is enabled (true), where name is
  /// one of the properties from the app settings table.
  /// @param name
  /// @return
  auto enabled(const String &name) -> bool {
    return settings[name].equalsIgnoreCase(True);
  }

  /// @brief Returns the value of name app setting, where name is one of the
  /// strings in the app settings table.
  /// @param name
  /// @return
  auto get(const String &name) -> String { return settings[name]; }

  /// @brief Assigns setting name to value. You may store any value that you
  /// want, but
  // certain names can be used to configure the behavior of the server. These
  // special
  /// names are listed in the app settings table.
  /// @param name
  /// @param value
  auto set(const String &name, const String &value) -> void {
    settings[name] = value;
  }

  /// @brief register the given template engine callback as ext.
  /// @param name
  /// @param value
  auto engine(const String &ext, const RenderEngineCallback callback) -> void {
    engines[ext] = callback;
  }

  /// @brief
  /// @param name
  /// @param callback
  auto on(const String &name, const MountCallback) -> void;

#pragma region HTTP_Methods

private:
  /// https://expressjs.com/en/guide/writing-middleware.html
  /// https://expressjs.com/en/guide/using-middleware.html

public:
  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto head(const String &path, Args... args) -> _Route & {
    return router_->head(path, args...);
  };

  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto get(const String &path, Args... args) -> _Route & {
    return router_->get(path, args...);
  };

  /// @brief
  /// @param path
  /// @param callbacks
  /// @param callback
  /// @return
  template <typename... Args>
  auto post(const String &path, Args... args) -> _Route & {
    return router_->post(path, args...);
  };

  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto put(const String &path, Args... args) -> _Route & {
    return router_->put(path, args...);
  };

  /// @brief Routes HTTP DELETE requests to the specified path with the
  /// specified callback functions. For more information, see the routing guide.
  /// @param path
  /// @param callback
  template <typename... Args>
  auto del(const String &path, Args... args) -> _Route & {
    return router_->del(path, args...);
  }

  /// @brief This method is like the standard app.METHOD() methods, except it
  /// matches all HTTP verbs.
  /// @param path
  /// @param callback
  template <typename... Args>
  auto all(const String &path, Args... args) -> _Route & {
    return router_->all(path, args...);
  }

#pragma endregion HTTP_Methods

  /// @brief Returns the canonical path of the app, a string.
  /// @return
  auto path() -> String;

  /// @brief Returns an instance of a single route, which you can then use to
  /// handle HTTP verbs with optional middleware. Use app.route() to avoid
  /// duplicate route names (and thus typo errors).
  auto route(const String &path) -> _Route &;

  /// @brief
  void listen(uint16_t port, const Callback startedCallback = nullptr);

  /// @brief
  auto run() -> void;

  /// @brief
  /// @param client
  void run(ClientType &client);

  /// @brief
  static void dada() { LOG_V(F("dada")); }
};

/// @brief
class _Request {
  friend class _Router;
  friend class _Express;

public:
  /// @brief
  ClientType &client;

  /// @brief This property holds a reference to the instance of the _Express
  /// application that is using the middleware.
  /// @return
  _Express &app;

  String uri{};

  /// @brief
  String body{};

  /// @brief
  bool fresh = true;

  /// @brief Contains the host derived from the Host HTTP header
  String host{};

  /// @brief Contains the hostname derived from the Host HTTP header
  String hostname{}; // without port

  /// @brief Contains the remote IP address of the request.
  IPAddress ip{};

  /// @brief Contains the remote IP address of the request.
  std::vector<IPAddress> ips{};

  /// @brief Contains the remote IP address of the request.
  std::vector<String> subdomains{};

  /// @brief intermediate pointer buffer for data callback
  _Route *route = nullptr;

  /// @brief Contains a string corresponding to the HTTP method of the request:
  /// GET, POST, PUT, and so on.
  String method{};

  /// @brief A Boolean property that is true if a TLS connection is established.
  ///  Equivalent to: (protocol === 'https')
  bool secure{};

  /// @brief
  std::map<String, String> headers;

  /// @brief Contains the path part of the request URL.
  String path{};

  /// @brief Contains the request protocol string: either http or (for TLS
  /// requests) https.
  String protocol{};

  /// @brief
  bool stale = false;

  /// @brief
  std::map<String, String> query;

  /// @brief This property is an object containing properties mapped to the
  /// named route “parameters”. For example, if you have the route /user/:name,
  /// then the “name” property is available as
  //  params[name]
  params_t params{};

public: /* Methods*/
  /// @brief Constructor
  _Request(_Express &, ClientType &);

  /// @brief Checks if the specified content types are acceptable, based on the
  /// request’s Accept HTTP header field. The method returns the best match, or
  /// if none of the specified content types is acceptable, returns false (in
  /// which case, the application should respond with 406 "Not Acceptable").
  auto accepts(const String &) -> bool;

  /// @brief Returns the matching content type if the incoming request’s
  /// “Content-Type” HTTP header field matches the MIME type specified by the
  /// type parameter. If the request has no body, returns null. Returns false
  /// otherwise.
  auto is(const String &) -> String;

  /// @brief Range header parser.
  /// The size parameter is the maximum size of the resource.
  /// The options parameter is an object that can have the following properties.
  auto range(const size_t & = 0) -> const Range &;

  /// @brief Returns the specified HTTP request header field (case-insensitive
  /// match).
  /// @param field
  /// @return
  auto get(const String &) -> String;

private:
  /// @brief
  /// @param client
  /// @return
  bool parse(ClientType &);

  /// @brief
  Range range_;

  /// @brief
  Method method_{};

  /// @brief
  /// @param data
  auto parseArguments(const String &) -> void;

  /// @brief
  /// @param text
  /// @return
  static auto urlDecode(const String &) -> String;
};

/// @brief
class _Response {
private:
  static void renderFile(ClientType &, Options *, const char *f,
                         const Write_Callback);

public:
  /// @brief
  const ClientType &client_;

  HttpStatus status_ = HttpStatus::NOT_FOUND;

  std::map<String, String> headers;

  /// Boolean property that indicates if the app sent HTTP headers for the
  /// response.
  bool headersSent{};

  /// @brief This property holds a reference to the instance of the _Express
  /// application that is using the middleware.
  /// @return
  _Express &app;

private:
  String body_{};

  /// @brief derefered rendering
  ContentCallback contentsCallback{};

  locals_t renderLocals{};

  String filename;

  Options *options = nullptr;

public:
  /// @brief
  /// @param client
  void evaluateHeaders(ClientType &);

  /// @brief
  /// @param client
  void sendBody(ClientType &, locals_t &);

  /// @brief
  void send();

public: /* Methods*/
  /// @brief Constructor
  _Response(_Express &, ClientType &);

  /// @brief Appends the specified value to the HTTP response header field. If
  /// the header is not already set, it creates the header with the specified
  /// value. The value parameter can be a string or an array. Note: calling
  /// res.set() after res.append() will reset the previously-set header value.
  /// @param field
  /// @param value
  /// @return
  auto append(const String &field, const String &value) -> _Response &;

  /// @brief Performs content-negotiation on the Accept HTTP header on the
  /// request object, when present. It uses req.accepts() to select a handler
  /// for the request, based on the acceptable types ordered by their quality
  /// values. If the header is not specified, the first callback is invoked.
  /// When no match is found, the server responds with 406 “Not Acceptable”, or
  /// invokes the default callback.
  ///
  /// The Content-Type response header is set when a callback is selected.
  /// However, you may alter this within the callback using methods such as
  /// res.set() or res.type().
  auto format() -> void;

  auto download(File &) -> void;

  auto cookie() -> void;

  auto clearCookie(const String &) -> void;

  /// @brief Ends the response process. This method actually comes from Node
  /// core, specifically the response.end() method of http.ServerResponse.
  /// @param data
  /// @param encoding
  /// @return
  auto end(Buffer *data = nullptr, const String &encoding = F(""))
      -> _Response &;

  /// @brief Ends the response process
  static void end();

  /// @brief Returns the HTTP response header specified by field. The match is
  /// case-insensitive.
  /// @return
  auto get(const String &field) -> String;

  /// @brief Sends a JSON response. This method sends a response (with the
  /// correct content-type) that is the parameter converted to a JSON string
  /// using JSON.stringify().
  /// @param body
  /// @return
  auto json(const String &body) -> void;

  /// @brief Sends the HTTP response.
  /// Optional parameters:
  /// @param view
  auto send(const String &body) -> _Response &;
  ;

  /// @brief Renders a view and sends the rendered HTML string to the client.
  /// Optional parameters:
  ///    - locals, an object whose properties define local variables for the
  ///    view.
  ///    - callback, a callback function. If provided, the method returns both
  ///    the
  ///      possible error and rendered string, but does not perform an automated
  ///      response. When an error occurs, the method invokes next(err)
  ///      internally.
  /// @param view
  auto render(File &, locals_t &) -> void;

  /// @brief .
  auto sendFile(const File &, Options *options = nullptr) -> void;

  /// @brief Sets the response HTTP status code to statusCode and sends the
  ///  registered status message as the text response body. If an unknown
  // status code is specified, the response body will just be the code number.
  /// @param statusCode
  auto sendStatus(const HttpStatus) -> void;

  /// @brief Sets the response’s HTTP header field to value
  /// @param field
  /// @param value
  /// @return
  auto set(const String &field, const String &value) -> _Response &;

  /// @brief Sends a JSON response. This method sends a response (with the
  /// correct content-type) that is the parameter converted to a JSON string
  /// using JSON.stringify().
  /// @param body
  /// @return
  auto status(const HttpStatus) -> _Response &;
};

/// @brief
class _Route {
public:
private:
  static const char delimiter = '/';

public: /// @brief
  DataCallback dataCallback_ = nullptr;

  /// @brief
  EndDataCallback endCallback_ = nullptr;

public:
  Method method = Method::UNDEFINED;

  String path{};

  std::vector<MiddlewareCallback> middlewares;

  // cache path splitting (avoid doing this for every request * number of paths)
  std::vector<PosLen> indices;

public:
  /// @brief
  _Route();

  /// @brief
  /// @param path
  auto splitToVector(const String &path) -> void;

  /// @brief
  /// @param path
  /// @return
  static auto splitToVector(const String &path, std::vector<PosLen> &poslens)
      -> void;

  /// @brief
  /// @param name
  /// @param callback
  auto on(const String &name, const DataCallback callback) -> void;

  /// @brief
  /// @param name
  /// @param callback
  auto on(const String &name, const EndDataCallback callback) -> void;
};

/// @brief
class _Router {
private:
  /// @brief The app.mountpath property contains the path patterns
  /// on which a sub-app was mounted.
  String mountpath{};

  /// @brief Application wide middlewares
  std::vector<MiddlewareCallback> middlewares{};

  /// @brief Application wide middlewares
  std::vector<ErrorCallback> errorHandlers{};

  /// @brief
  _Router *parent = nullptr;

  /// @brief
  std::map<String, _Router *> routers_{};

  /// @brief routes
  std::vector<_Route *> routes{};

  /// @brief
  static bool gotoNext;

public:
  _Router();

#pragma region HTTP_Methods

private:
  /// @brief
  /// @param path
  /// @param pathItems
  /// @param requestPath
  /// @param requestPathItems
  /// @param params
  /// @return
  static auto match(const String &path, const std::vector<PosLen> &pathItems,
                    const String &requestPath,
                    const std::vector<PosLen> &requestPathItems,
                    params_t &params) -> bool;

  /// @brief
  /// @param req
  /// @param res
  auto evaluate(_Request &, _Response &) -> bool;

  /// https://expressjs.com/en/guide/writing-middleware.html
  /// https://expressjs.com/en/guide/using-middleware.html

  std::vector<MiddlewareCallback> tmpMiddlewares;

  void addMiddleware(MiddlewareCallback middleware) {
    if (nullptr != middleware)
      tmpMiddlewares.push_back(middleware);
  }

  template <typename... Args>
  void addMiddleware(MiddlewareCallback middleware, Args... tail) {
    if (nullptr != middleware)
      tmpMiddlewares.push_back(middleware);
    addMiddleware(tail...);
  }

  template <typename... Args>
  void addMiddleware(std::vector<MiddlewareCallback> middlewares,
                     Args... tail) {
    for (auto middleware : middlewares)
      if (nullptr != middleware)
        addMiddleware(middleware);
    addMiddleware(tail...);
  }

  /// @brief
  /// @param method
  /// @param path
  /// @param middlewares
  /// @param middleware
  /// @return
  auto METHOD(const Method, const String &path,
              const std::vector<MiddlewareCallback>) -> _Route &;

public:
  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto head(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::HEAD, path, tmpMiddlewares);
  };

  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto get(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::GET, path, tmpMiddlewares);
  };

  /// @brief
  /// @param path
  /// @param callbacks
  /// @param callback
  /// @return
  template <typename... Args>
  auto post(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::POST, path, tmpMiddlewares);
  };

  /// @brief
  /// @param path
  /// @param callback
  /// @return
  template <typename... Args>
  auto put(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::PUT, path, tmpMiddlewares);
  };

  /// @brief Routes HTTP DELETE requests to the specified path with the
  /// specified callback functions. For more information, see the routing guide.
  /// @param path
  /// @param callback
  template <typename... Args>
  auto del(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::DELETE, path, tmpMiddlewares);
  }

  /// @brief This method is like the standard app.METHOD() methods, except it
  /// matches all HTTP verbs.
  /// @param path
  /// @param callback
  template <typename... Args>
  auto all(const String &path, Args... args) -> _Route & {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::ALL, path, tmpMiddlewares);
  }

  template <typename... Args> _Route &adder(const String &path, Args... args) {
    tmpMiddlewares.clear();
    addMiddleware(args...);
    return METHOD(Method::HEAD, path, tmpMiddlewares);
  };

#pragma endregion HTTP_Methods

  void param(){/* NOT IMPLEMENTED */};

  /// @brief Returns an instance of a single route, which you can then use to
  /// handle HTTP verbs with optional middleware. Use app.route() to avoid
  /// duplicate route names (and thus typo errors).
  _Route &route(const String &path);

  /// @brief
  auto dispatch(_Request &, _Response &) -> void;

#pragma region Middleware

  /// @brief
  /// @param errorCallback
  /// @return
  auto use(const ErrorCallback) -> void;

  /// @brief
  /// @param errorCallback
  /// @return
  auto use(const std::vector<ErrorCallback>) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const MiddlewareCallback) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const std::vector<MiddlewareCallback>) -> void;

  /// @brief
  /// @param middleware
  /// @return
  auto use(const String &path, const MiddlewareCallback) -> void;

  /// @brief The app.mountpath property contains one or more path patterns on
  /// which a sub-app was mounted.
  /// @param mount_path
  /// @param other
  /// @return
  auto use(const String &mount_path, _Router &) -> void;

  /// @brief The app.mountpath property contains one or more path patterns on
  /// which a sub-app was mounted.
  /// @param mount_path
  /// @return
  auto use(const String &mount_path) -> void;

#pragma endregion Middleware
};

END_EXPRESS_NAMESPACE

#define EXPRESS_CREATE_NAMED_INSTANCE(Name)                                    \
  typedef _Express express;                                                    \
  typedef _Route route;                                                        \
  typedef _Request request;                                                    \
  typedef _Response response;                                                  \
  typedef _Error Error;                                                        \
  express Name;

#define EXPRESS_CREATE_INSTANCE() EXPRESS_CREATE_NAMED_INSTANCE(app);
