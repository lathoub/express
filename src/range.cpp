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
};

/// @brief parse content:
/// bytes=100-1023
/// Range: bytes=0-50, 100-150 // not supported
auto _Range::parse(const String &str) -> void {
  type = F("");
  start = 0;
  end = 0;

  if (str.length() < 3)
    return;

  auto index = str.indexOf('=');
  if (index < 0)
    return;

  type = str.substring(0, index);
  LOG_V(".type", type);

  auto ranges = str.substring(index + 1);
  //LOG_V(".ranges", ranges);

  index = ranges.indexOf(',');
  //LOG_V(".index", index);
  while (index >= 0) {

    auto range = ranges.substring(0, index);
    //LOG_V("range", range);

    ranges = ranges.substring(index + 1);
    //LOG_V("ranges", ranges);

    index = range.indexOf('-');
    if (index >= 0) {
      auto startStr = range.substring(0, index);
      LOG_V("startStr", startStr);
      auto endStr = range.substring(index + 1);
      LOG_V("endStr", endStr);
    }

    index = ranges.indexOf(',');
    //LOG_V(":index", index);
  }

  index = ranges.indexOf('-');
  if (index >= 0) {
    auto startStr = ranges.substring(0, index);
    LOG_V("startStr", startStr);
    auto endStr = ranges.substring(index + 1);
    LOG_V("endStr", endStr);
  }

  /*
    //----
    auto startEnd = str.substring(index + 1);
    index = startEnd.indexOf('-');
    if (index < 0)
      return;

    start = startEnd.substring(0, index).toInt();

    auto endStr = startEnd.substring(index + 1);
    endStr.trim();
    end = (endStr.length() == 0) ? -1 : endStr.toInt();
  */
  // type = F("bytes");
}

auto _Range::toString() -> String {
  String tmp = F("bytes=");
  tmp += start;
  tmp += F("-");
  if (end >= 0)
    tmp += end;

  return tmp;
}

END_EXPRESS_NAMESPACE
