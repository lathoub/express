/*!
 *  @file       route.cpp
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

#include "Express.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief
Router::Router() {}

/// @brief Returns an instance of a single route, which you can then use to
/// handle HTTP verbs with optional middleware. Use app.route() to avoid
/// duplicate route names (and thus typo errors).
Route &Router::route(const String &path) {
  const auto route = new Route();
  route->path = path;

  route->splitToVector(route->path);
  // Add to collection
  routes.push_back(route);

  return *route;
}

#pragma region Middleware

/// @brief
/// @param middleware
/// @return
auto Router::use(const MiddlewareCallback middleware) -> void // TODO, args...
{
  middlewares.push_back(middleware);
}

/// @brief
/// @param middleware
/// @return
auto Router::use(const std::vector<MiddlewareCallback> middlewares)
    -> void // TODO, args...
{
  for (auto middleware : middlewares)
    this->middlewares.push_back(middleware);
}

/// @brief
/// @param middleware
/// @return
auto Router::use(const String &path, const MiddlewareCallback middleware)
    -> void // TODO, args...
{
  // TODO
}

/// @brief The app.mountpath property contains one or more path patterns on
/// which a sub-app was mounted.
/// @param mount_path
/// @param other
/// @return
auto Router::use(const String &mount_path, Router &other) -> void {
  LOG_I(F("use mountPath:"), mount_path);

  other.mountpath = mount_path;
  other.parent = this;
  mountPaths[other.mountpath] = &other;
}

/// @brief The app.mountpath property
/// @param mount_path
/// @return
auto Router::use(const String &mount_path) -> void { mountpath = mount_path; }

#pragma endregion Middleware

END_EXPRESS_NAMESPACE
