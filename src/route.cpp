    /*!
 *  @file       route.h
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

#include <Arduino.h>
#include "Express.h"

BEGIN_EXPRESS_NAMESPACE

    /// @brief
    Route::Route()
    {
    }

    auto Route::splitToVector(const String &path) -> void
    {
        splitToVector(path, indices);
    }

    /// @brief
    /// @param path
    /// @return
    auto Route::splitToVector(const String &path, std::vector<PosLen> &poslens) -> void
    {
        size_t p = 0, i = 1;
        for (; i < path.length(); i++)
        {
            if (path.charAt(i) == delimiter)
            {
                poslens.push_back({p, i - p});
                p = i;
            }
        }
        poslens.push_back({p, i - p});
    }

    /// @brief
    /// @param name
    /// @param callback
    auto Route::on(const String &name, const DataCallback callback) -> void
    {
        LOG_I(F("register data callback"), name);
        dataCallback_ = callback;
        // return *this;
    }

    /// @brief
    /// @param name
    /// @param callback
    auto Route::on(const String &name, const EndDataCallback callback) -> void
    {
        LOG_I(F("register end callback"), name);
        endCallback_ = callback;
        //  return *this;
    }

END_EXPRESS_NAMESPACE
