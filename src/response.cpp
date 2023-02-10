/*!
 *  @file       response.hpp
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

#include "response.h"

BEGIN_EXPRESS_NAMESPACE

/// @brief 
/// @param app 
/// @param client 
/// @return 
Response::Response(Express *app, EthernetClient &client)
    : app_(app), client_(client)
{
}

/// @brief 
/// @param field 
/// @param value 
/// @return 
auto Response::append(const String &field, const String &value) -> Response &
{
    for (auto [key, header] : headers_)
    {
        if (field.equalsIgnoreCase(key))
        {
            // Appends the specified value to the HTTP response header
            header += value;
            return *this;
        }
    }

    // not found, creates the header with the specified value
    headers_[field] = value;

    return *this;
}

/// @brief
/// @param client
void Response::evaluateHeaders(EthernetClient &client)
{
    if (body_ && body_ != F(""))
        headers_[ContentLength] = body_.length();

//    if (app_->settings.count(XPoweredBy) > 0)
//        headers_[XPoweredBy] = app_->settings[XPoweredBy];

    headers_[F("connection")] = F("close");
}

/// @brief
/// @param client
void Response::sendBody(EthernetClient &client, locals_t &locals)
{
    if (body_ && body_ != F(""))
        client.println(body_.c_str());
    else if (contentsCallback_)
    {
        int lastDot = filename_.lastIndexOf('.');
        auto ext = filename_.substring(lastDot + 1);

//        auto engineName = app_.settings[F("view engine")];
//        if (engineName.equals(ext))
//        {
//            auto engine = app_.engines[engineName];
//            if (engine)
//                engine(client, locals, contentsCallback_());
//        }
//        else
            renderFile(client, contentsCallback_()); // default renderer
    }
}

/// @brief
void Response::send()
{
    auto &client = const_cast<EthernetClient &>(client_);

    client.print(F("HTTP/1.1 "));
    client.println(status_);

    // Add to headers
    evaluateHeaders(client);

    LOG_V(F("Headers:"));
    // Send headers
    for (auto [first, second] : headers_)
    {
        LOG_V(first, second);

        client.print(first);
        client.print(": ");
        client.println(second);
    }
    client.println();

    sendBody(client, renderLocals_);

    client.setConnectionTimeout(5);
    client.stop();
}

END_EXPRESS_NAMESPACE
