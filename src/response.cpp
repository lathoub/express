/*!
 *  @file       response.cpp
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
/// @param app
/// @param client
/// @return
Response::Response(Express &express, ClientType &client)
    : app(express), client_(client) {}

/// @brief  // default renderer
/// @param client
/// @param f
void Response::renderFile(ClientType &client, const char *f) {
  size_t i = 0;
  size_t start = 0;
  while (f[start + i] != '\0') {
    while (f[start + i] != '\n' && f[start + i] != '\0')
      i++;

    client.write(f + start, i);
    client.write('\n');

    if (f[start + i] == '\0')
      break;

    start += i + 1;
    i = 0;
  }
}

/// @brief
/// @param field
/// @param value
/// @return
auto Response::append(const String &field, const String &value) -> Response & {
  for (auto [key, header] : headers) {
    if (field.equalsIgnoreCase(key)) {
      // Appends the specified value to the HTTP response header
      header += value;
      return *this;
    }
  }

  // not found, creates the header with the specified value
  headers[field] = value;

  return *this;
}

/// @brief Ends the response process. This method actually comes from Node core,
/// specifically the response.end() method of http.ServerResponse.
/// @param data
/// @param encoding
/// @return
auto Response::end(Buffer *buffer, const String &encoding) -> Response & {
  if (buffer) {
    body_ = buffer->toString();

    LOG_V(body_);
  }

  return *this;
}

/// @brief Returns the HTTP response header specified by field. The match is
/// case-insensitive.
/// @return
auto Response::get(const String &field) -> String {
  for (auto [key, header] : headers) {
    if (field.equalsIgnoreCase(key))
      return header;
  }
  return "";
}

/// @brief Sends a JSON response. This method sends a response (with the correct
/// content-type) that is the parameter converted to a JSON string using
/// JSON.stringify().
/// @param body
/// @return
auto Response::json(const String &body) -> void {
  body_ = body;

  set(ContentType, ApplicationJson);
  // QUESTION: set content-length here?
}

/// @brief Sends the HTTP response.
/// Optional parameters:
/// @param view
auto Response::send(const String &body) -> Response & {
  body_ = body;

  return *this;
}

/// @brief Renders a view and sends the rendered HTML string to the client.
/// Optional parameters:
///    - locals, an object whose properties define local variables for the view.
///    - callback, a callback function. If provided, the method returns both the
///      possible error and rendered string, but does not perform an automated
///      response. When an error occurs, the method invokes next(err)
///      internally.
/// @param view
auto Response::render(File &file, locals_t &locals) -> void {
  // NOTE: don't render here just yet (status and headers need to be send first)
  // so store a backpointer that can be called in the sendBody function.
  // set this here already, so it gets send out as part of the headers

  contentsCallback = file.contentsCallback;
  renderLocals = locals; // TODO: check if this copies??
  filename = file.filename;

  set(ContentType, F("text/html"));
}

/// @brief .
auto Response::sendFile(File &file, Options *options) -> void {
  if (options) {
    for (auto [first, second] : options->headers) {
      LOG_V(first, second);
      set(first, second);
    }
  }

  contentsCallback = file.contentsCallback;
  filename = file.filename;
}

/// @brief Sets the response HTTP status code to statusCode and sends the
///  registered status message as the text response body. If an unknown
// status code is specified, the response body will just be the code number.
/// @param statusCode
auto Response::sendStatus(const HttpStatus statusCode) -> void {
  status_ = statusCode;
}

/// @brief Sets the response’s HTTP header field to value
/// @param field
/// @param value
/// @return
auto Response::set(const String &field, const String &value) -> Response & {
  for (auto [key, header] : headers) {
    if (field.equalsIgnoreCase(key)) {
      // Appends the specified value to the HTTP response header
      header = value;
      return *this;
    }
  }

  // not found, creates the header with the specified value
  headers[field] = value;

  return *this;
}

/// @brief
/// @param body
/// @return
auto Response::status(const HttpStatus status) -> Response & {
  status_ = status;

  return *this;
}

/// @brief
/// @param client
void Response::evaluateHeaders(ClientType &client) {
  if (body_ && body_ != F(""))
    headers[ContentLength] = body_.length();

  if (app.settings.count(XPoweredBy) > 0)
    headers[XPoweredBy] = app.settings[XPoweredBy];

  headers[F("connection")] = F("close");
}

/// @brief
/// @param client
void Response::sendBody(ClientType &client, locals_t &locals) {
  LOG_V(F("sendBody"));

  // if we already have a body, send that over
  if (body_ && body_ != F(""))
    client.println(body_.c_str());
  else if (contentsCallback) {
    // a request to generate the body was issued earlier,
    // execute it here.
    int lastDot = filename.lastIndexOf('.');
    auto ext = filename.substring(lastDot + 1);

    LOG_V(F("ext"), ext);

    auto engineName = app.settings[F("view engine")];
    LOG_V(F("engineName"), engineName);
    if (engineName.equals(ext)) {
      auto engine = app.engines[engineName];
      if (engine)
        engine(client, locals, contentsCallback());
    } else
      renderFile(client, contentsCallback()); // default renderer
  }
}

/// @brief
void Response::send() {
  auto &client = const_cast<ClientType &>(client_);

  client.print(F("HTTP/1.1 "));
  client.println(status_);

  // Add to headers
  evaluateHeaders(client);

  LOG_V(F("Headers:"));
  // Send headers
  for (auto [first, second] : headers) {
    LOG_V(first, second);

    client.print(first);
    client.print(": ");
    client.println(second);
  }
  client.println();

  sendBody(client, renderLocals);
}

END_EXPRESS_NAMESPACE
