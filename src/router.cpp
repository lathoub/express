/*!
 *  @file       route.cpp
 *  Project     Arduino _Express Library
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

#include "Express.h"

BEGIN_EXPRESS_NAMESPACE

// static declarations
bool _Router::gotoNext{};

/// @brief Constructor
_Router::_Router() { LOG_T(F("_Router contructor")); }

/// @brief Returns an instance of a single route, which you can then use to
/// handle HTTP verbs with optional middleware. Use app.route() to avoid
/// duplicate route names (and thus typo errors).
_Route &_Router::route(const String &path) {

  LOG_T(F("New _Route"), path);

  const auto route = new _Route();
  route->path = path;

  route->splitToVector(route->path);
  // Add to collection
  routes.push_back(route);

  return *route;
}

/// @brief
/// @param path
/// @param pathItems
/// @param requestPath
/// @param requestPathItems
/// @param params
/// @return
auto _Router::match(const String &path, const std::vector<PosLen> &pathItems,
                   const String &requestPath,
                   const std::vector<PosLen> &requestPathItems,
                   params_t &params) -> bool {
  if (requestPathItems.size() != pathItems.size()) {
    return false;
  }

  for (size_t i = 0; i < requestPathItems.size(); i++) {
    const auto &ave = requestPathItems[i];
    const auto &bve = pathItems[i];

    if (path.charAt(bve.pos + 1) == ':') // Note: : comes right after /
    {
      auto name = path.substring(bve.pos + 2,
                                 bve.pos + bve.len); // Note: + 2 to offset /:
      name.toLowerCase();
      const auto value = requestPath.substring(
          ave.pos + 1, ave.pos + ave.len); // Note + 1 to offset /
      params[name] = value;
    } else {
      if (requestPath.substring(ave.pos, ave.pos + ave.len) !=
          path.substring(bve.pos, bve.pos + bve.len)) {
        return false;
      }
    }
  }

  return true;
}

/// @brief
/// @param req
/// @param res
/// @param next
/// @return
auto _Router::evaluate(Request &req, Response &res) -> bool {
  LOG_V(F("_Router::evaluate, req.uri:"), req.uri, F("routes:"), routes.size());

  std::vector<PosLen> req_indices{};
  _Route::splitToVector(req.uri, req_indices);

  for (auto route : routes) {
    if ((route->method == Method::ALL || req.method == route->method) &&
        match(route->path, route->indices, req.uri, req_indices, req.params)) {
      res.status_ = HttpStatus::OK;
      req.route = route;

      // run the route wide middlewares
      for (const auto middleware : route->middlewares) {
        gotoNext = false;
        try {
          middleware(req, res, [gotoNext](const _Error *error) {
            if (error) // reconstruct error message in new object
              throw new _Error(error->message);
            gotoNext = true;
          });
        } catch (_Error *error) {
          res.status(HttpStatus::SERVER_ERROR);
          for (const auto errorHandler : errorHandlers) {
            errorHandler(*error, req, res,
                         [gotoNext](const _Error *error) { gotoNext = true; });
            if (!gotoNext)
              break;
          }
          return false;
        }
        if (!gotoNext)
          break;
      }

      return true;
    }
  }

  LOG_V(F("evaluate child routers"), routers_.size());
  for (auto [mountpath, _Router] : routers_) {
    if (_Router->evaluate(req, res))
      return true;
  }

  return false;
}

/// @brief
auto _Router::dispatch(Request &req, Response &res) -> void {
  /// @brief run the _Router wide middlewares
  gotoNext = true;
  for (const auto middleware : middlewares) {
    gotoNext = false;
    middleware(req, res, [gotoNext](const _Error *error) { gotoNext = true; });
    if (!gotoNext)
      break;
  }

  if (gotoNext)
    evaluate(req, res);
}

#pragma region Middleware

/// @brief
/// @tparam ArrayType
/// @tparam ArraySize
/// @param method
/// @param path
/// @param middlewares
/// @param middleware
/// @return
auto _Router::METHOD(const Method method, const String &path,
                    const std::vector<MiddlewareCallback> middlewares)
    -> _Route & {

  auto _path = path;
  auto _mountpath = mountpath;

  if (_path == F("/"))
    _path = F("");
  if (_mountpath == F("/"))
    _mountpath = F("");

  _path = _mountpath + _path;
  _path.trim();

  LOG_I(F("METHOD:"), method, F("path:"), _path, F("#middlewares:"),
        middlewares.size());

  const auto route = new _Route();
  route->method = method;
  route->path = _path;
  route->middlewares = middlewares; // copy the vector

  route->splitToVector(route->path);
  // Add to collection
  routes.push_back(route);

  return *route;
}

/// @brief
/// @param middleware
/// @return
auto _Router::use(const ErrorCallback errorHandler) -> void // TODO, args...
{
  this->errorHandlers.push_back(errorHandler);
}

/// @brief
/// @param middleware
/// @return
auto _Router::use(const std::vector<ErrorCallback> errorHandlers)
    -> void // TODO, args...
{
  for (auto errorHandler : errorHandlers)
    this->errorHandlers.push_back(errorHandler);
}

/// @brief
/// @param middleware
/// @return
auto _Router::use(const MiddlewareCallback middleware) -> void // TODO, args...
{
  middlewares.push_back(middleware);
}

/// @brief
/// @param middleware
/// @return
auto _Router::use(const std::vector<MiddlewareCallback> middlewares)
    -> void // TODO, args...
{
  for (auto middleware : middlewares)
    this->middlewares.push_back(middleware);
}

/// @brief
/// @param middleware
/// @return
auto _Router::use(const String &path, const MiddlewareCallback middleware)
    -> void // TODO, args...
{
  // TODO
}

/// @brief The app.mountpath property contains one or more path patterns on
/// which a sub-app was mounted.
/// @param mountpath
/// @param other
/// @return
auto _Router::use(const String &mountpath, _Router &otherRouter) -> void {
  LOG_I(F("otherRouter:"), mountpath, otherRouter.routes.size());

  otherRouter.mountpath = mountpath;
  otherRouter.parent = this;
  routers_[otherRouter.mountpath] = &otherRouter;
}

/// @brief The app.mountpath property
/// @param mountpath
/// @return
auto _Router::use(const String &mountpath) -> void {
  this->mountpath = mountpath;
}

#pragma endregion Middleware

END_EXPRESS_NAMESPACE
