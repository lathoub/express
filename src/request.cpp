/*!
 *  @file       request.cpp
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

_Request::_Request(_Express &express, ClientType &ec)
    : app(express), client(ec), method(Method::UNDEFINED) {
  LOG_T(F("_Request constructor"));
  parse(client);
}

/// @brief Checks if the specified content types are acceptable, based on the
/// request’s Accept HTTP header field. The method returns the best match, or if
/// none of the specified content types is acceptable, returns false (in which
/// case, the application should respond with 406 "Not Acceptable").
auto _Request::accepts(const String &types) -> bool { return false; }

/// @brief Returns the matching content type if the incoming request’s
/// “Content-Type” HTTP header field matches the MIME type specified by the
/// type parameter. If the request has no body, returns null. Returns false
/// otherwise.
auto _Request::is(const String &types) -> String { return F(""); }

/// @brief Range header parser.
/// The size parameter is the maximum size of the resource.
/// The options parameter is an object that can have the following properties.
auto _Request::range(const size_t &size) -> const Range & {
  range_.ranges.clear();

  auto str = get(F("range"));

  if (str.length() < 3)
    return range_;

  auto index = str.indexOf('=');
  if (index < 0)
    return range_;

  range_.type = str.substring(0, index);  // before = (type)
  auto ranges = str.substring(index + 1); // after =

  index = ranges.indexOf(',');
  while (index >= 0) {
    auto range = ranges.substring(0, index); // before ,
    ranges = ranges.substring(index + 1);    // after ,

    index = range.indexOf('-');
    if (index >= 0)
      range_.ranges.push_back({range.substring(0, index).toInt(),
                               range.substring(index + 1).toInt()});

    index = ranges.indexOf(',');
  }

  index = ranges.indexOf('-');
  if (index >= 0)
    range_.ranges.push_back({ranges.substring(0, index).toInt(),
                             ranges.substring(index + 1).toInt()});

  // if last end is zero, add maxint
  if (range_.ranges.back().end == 0)
    range_.ranges.back().end = INT_MAX;

  // TODO check for overlap
  // throw new _Error(HttpStatus::RANGE_NOT_SATISFIABLE);

  return range_;
};

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

  method_ = Method::UNDEFINED;
  uri = "";
  hostname = "";
  body = "";
  params.clear();
  headers.clear();
  query.clear();

  protocol = F("http");
  secure = (protocol == F("https"));

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  auto addr_start = reqStr.indexOf(' ');
  auto addr_end = reqStr.indexOf(' ', addr_start + 1);

  if (addr_start == -1 || addr_end == -1) {
    LOG_V(F("_parseRequest: Invalid request: "), reqStr);
    method_ = Method::ERROR;
    return false;
  }

  method = reqStr.substring(0, addr_start);
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

  method_ = Method::GET;
  if (method == F("HEAD"))
    method_ = Method::HEAD;
  else if (method == "POST")
    method_ = Method::POST;
  else if (method == "DELETE")
    method_ = Method::DELETE;
  else if (method == "OPTIONS")
    method_ = Method::OPTIONS;
  else if (method == "PUT")
    method_ = Method::PUT;
  else if (method == "PATCH")
    method_ = Method::PATCH;

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
  }

  // always present
  host = headers[F("host")];

  if (app.disabled(F("trust proxy"))) {
    auto index = host.indexOf(':');
    hostname = host.substring(0, index);
    ip = client.remoteIP();
    // ip / ips?
  } else {
    auto index = headers[F("x-forwarded-for")].indexOf(':');
    hostname = headers[F("x-forwarded-for")].substring(0, index); // left-most
    ip = client.remoteIP();
    // ip / ips?
  }

  LOG_V(F("Host:"), host);
  LOG_V(F("Hostname:"), hostname);
  LOG_V(F("Method:"), method);
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
