/*!
 *  @file       request.cpp
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

_Request::_Request(_Express &_Express, ClientType &ec)
    : app(_Express), client(ec), method(Method::UNDEFINED) {
  LOG_T(F("_Request constructor"));
  parse(client);
}

/// @brief Checks if the specified content types are acceptable, based on the
/// request’s Accept HTTP header field. The method returns the best match, or if
/// none of the specified content types is acceptable, returns false (in which
/// case, the application should respond with 406 "Not Acceptable").
auto _Request::accepts(const String &types) -> bool { return false; }

/// @brief Returns the specified HTTP request header field (case-insensitive
/// match).
/// @param field
/// @return
auto _Request::get(const String &field) -> String {
  for (auto [key, header] : headers) {
    if (field.equalsIgnoreCase(key))
      return header;
  }

  static String empty{};
  return empty;
}

/// @brief
/// @param client
/// @return
bool _Request::parse(ClientType &client) {
  LOG_V(F("_Request::Parse"));

  // Read the first line of HTTP request
  String reqStr = client.readStringUntil('\r');
  client.readStringUntil('\n');

  LOG_V(F("First line"), reqStr);

  method = Method::UNDEFINED;
  uri = "";
  hostname = "";
  body = "";
  params.clear();
  headers.clear();
  query.clear();

  protocol = F("http");
  secure = (protocol == F("https"));
  ip = client.remoteIP();

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  auto addr_start = reqStr.indexOf(' ');
  auto addr_end = reqStr.indexOf(' ', addr_start + 1);

  if (addr_start == -1 || addr_end == -1) {
    LOG_V(F("_parseRequest: Invalid request: "), reqStr);
    method = Method::ERROR;
    return false;
  }

  auto method_str = reqStr.substring(0, addr_start);
  auto url = reqStr.substring(addr_start + 1, addr_end);
  auto version_end = reqStr.substring(addr_end + 8);
  //  version = atoi(versionEnd.c_str());
  String search_str = "";
  auto has_search = url.indexOf('?');

  if (has_search != -1) {
    search_str = url.substring(has_search + 1);
    url = url.substring(0, has_search);
  }

  uri = url;
  if (uri == F("/"))
    uri = F("");

  method = Method::GET;
  if (method_str == F("HEAD"))
    method = Method::HEAD;
  else if (method_str == "POST")
    method = Method::POST;
  else if (method_str == "DELETE")
    method = Method::DELETE;
  else if (method_str == "OPTIONS")
    method = Method::OPTIONS;
  else if (method_str == "PUT")
    method = Method::PUT;
  else if (method_str == "PATCH")
    method = Method::PATCH;

  // parse headers
  while (true) {
    reqStr = client.readStringUntil('\r');
    client.readStringUntil('\n');

    if (reqStr == "")
      break; // no more headers

    auto header_div = reqStr.indexOf(':');

    if (header_div == -1)
      break;

    auto header_name = reqStr.substring(0, header_div);
    header_name.toLowerCase();
    auto header_value = reqStr.substring(header_div + 2);
    headers[header_name] = header_value; // TODO keep all headers or just a few?

    if (header_name.equalsIgnoreCase(F("Host")))
      hostname = header_value;
  }

  LOG_V(F("Method:"), method_str);
  LOG_V(F("Uri:"), uri);

  LOG_V(F("Headers (all forced to lowercase)"));
  for (auto [header, value] : headers)
    LOG_V(F("header:"), header, F("value:"), value);

  parseArguments(search_str);

  return true;
}

/// @brief
/// @param data
auto _Request::parseArguments(const String &data) -> void {
  if (data.length() == 0)
    return;

  int arg_count = 1;

  for (int i = 0; i < static_cast<int>(data.length());) {
    i = data.indexOf('&', i);

    if (i == -1)
      break;

    ++i;
    ++arg_count;
  }

  int pos = 0;

  for (int iarg = 0; iarg < arg_count;) {
    int equal_sign_index = data.indexOf('=', pos);
    int next_arg_index = data.indexOf('&', pos);

    if ((equal_sign_index == -1) ||
        ((equal_sign_index > next_arg_index) && (next_arg_index != -1))) {
      if (next_arg_index == -1)
        break;

      pos = next_arg_index + 1;
      continue;
    }

    String key = urlDecode(data.substring(pos, equal_sign_index));
    key.toLowerCase();
    String value =
        urlDecode(data.substring(equal_sign_index + 1, next_arg_index));
    query[key] = value;

    ++iarg;

    if (next_arg_index == -1)
      break;

    pos = next_arg_index + 1;
  }

  LOG_V(F("Query Arguments"));
  for (auto [argument, value] : query)
    LOG_V(F("argument:"), argument, F("value:"), value);
}

/// @brief
/// @param text
/// @return
auto _Request::urlDecode(const String &text) -> String {
  String decoded = "";
  char temp[] = "0x00";
  unsigned int len = text.length();
  unsigned int i = 0;

  while (i < len) {
    char decoded_char;
    char encoded_char = text.charAt(i++);

    if ((encoded_char == '%') && (i + 1 < len)) {
      temp[2] = text.charAt(i++);
      temp[3] = text.charAt(i++);

      decoded_char = strtol(temp, NULL, 16);
    } else {
      decoded_char = encoded_char == '+' ? ' ' : encoded_char;
    }

    decoded += decoded_char;
  }

  return decoded;
}

END_EXPRESS_NAMESPACE
