/*!
 *  @file       range.cpp
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

/// @brief Constructor
_Range::_Range() {
  start = 0;
  end = 0;
}

/// @brief parse content:
/// bytes=100-1023
/// Range: bytes=0-50, 100-150 // not supported
auto _Range::parse(const String &str) -> void {
  auto index = str.indexOf('=');
  type = str.substring(0, index);
  auto startEnd = str.substring(index + 1);
  index = startEnd.indexOf('-');
  start = startEnd.substring(0, index).toInt();
  end = startEnd.substring(index + 1).toInt();
}

END_EXPRESS_NAMESPACE
