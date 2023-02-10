/*!
 *  @file       request.hpp
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

#include "request.h"

BEGIN_EXPRESS_NAMESPACE

Request::Request(Express *app, EthernetClient &client)
    : app_(app), client_(client), method(Method::UNDEFINED)
{
    parse(client);
}

/// @brief Checks if the specified content types are acceptable, based on the request’s Accept HTTP
/// header field. The method returns the best match, or if none of the specified content types is
/// acceptable, returns false (in which case, the application should respond with 406 "Not Acceptable").
auto Request::accepts(const String &types) -> bool
{
    return false;
}

END_EXPRESS_NAMESPACE
