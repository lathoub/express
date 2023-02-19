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
_Response::_Response(_Express &_Express, ClientType &client)
    : app(_Express), client_(client) {
  headersSent = false;
  LOG_T(F("_Response constructor"));
}

/// @brief  // default renderer. Send content in chuncks for x bytes
/// @param client
/// @param f
void _Response::renderFile(ClientType &client, Options *options,
                           const char *f) {
  LOG_V(F("default renderer"), (options) ? F("with options.") : F(""));

  const size_t maxChunkLen = 2048;

  _Range range;
  if (options && options->acceptRanges)
    range.parse(options->headers[F("range")]);

  const size_t until = (range.end <= 0)           ? strlen(f) - 1
                       : (range.end >= strlen(f)) ? strlen(f) - 1
                                                  : range.end;
  LOG_V("until", until);
  LOG_V("range", range.start, range.end);

  size_t i = range.start;
  while (i <= until) {
    auto remaining = (i + maxChunkLen <= until) ? maxChunkLen : until - i + 1;
    LOG_V("write", i, remaining);
    client.write(f + i, remaining);
    i += remaining;
  }
}

/// @brief
/// @param field
/// @param value
/// @return
auto _Response::append(const String &field, const String &value)
    -> _Response & {
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

/// @brief
/// @return
auto _Response::format() -> void{
    // TODO
};

/// @brief
/// @return
auto _Response::download(File &file) -> void {
  contentsCallback = file.contentsCallback;
  filename = file.filename;
  headers[F("Content-Disposition")] = F("attachment; filename=cool.html");
};

/// @brief
/// @return
auto _Response::cookie() -> void{
    // TODO
};

/// @brief
/// @return
auto _Response::clearCookie(const String &name) -> void{
    // TODO
};

/// @brief Ends the response process. This method actually comes from Node core,
/// specifically the response.end() method of http.ServerResponse.
/// @param data
/// @param encoding
/// @return
auto _Response::end(Buffer *buffer, const String &encoding) -> _Response & {
  if (buffer) {
    body_ = buffer->toString();

    LOG_V(body_);
  }

  return *this;
}

/// @brief Returns the HTTP response header specified by field. The match is
/// case-insensitive.
/// @return
auto _Response::get(const String &field) -> String {
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
auto _Response::json(const String &body) -> void {
  body_ = body;

  set(ContentType, ApplicationJson);
  // QUESTION: set content-length here?
}

/// @brief Sends the HTTP response.
/// Optional parameters:
/// @param view
auto _Response::send(const String &body) -> _Response & {
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
/// @param file
/// @param locals
auto _Response::render(File &file, locals_t &locals) -> void {
  // NOTE: don't render here just yet (status and headers need to be send first)
  // so store a backpointer that can be called in the sendBody function.
  // set this here already, so it gets send out as part of the headers

  contentsCallback = file.contentsCallback;
  renderLocals = locals; // TODO: check if this copies??
  filename = file.filename;

  set(ContentType, F("text/html"));
}

/// @brief .
auto _Response::sendFile(const File &file, Options *options) -> void {
  this->contentsCallback = file.contentsCallback;
  this->filename = file.filename;
  if (options)
    this->options = new Options(options);

  if (options) {
    LOG_V(F("sendFile options"), this->options->acceptRanges,
          this->options->headers[F("range")]);
  }
}

/// @brief Sets the response HTTP status code to statusCode and sends the
///  registered status message as the text response body. If an unknown
// status code is specified, the response body will just be the code number.
/// @param statusCode
auto _Response::sendStatus(const HttpStatus statusCode) -> void {
  status_ = statusCode;
}

/// @brief Sets the response’s HTTP header field to value
/// @param field
/// @param value
/// @return
auto _Response::set(const String &field, const String &value) -> _Response & {
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
auto _Response::status(const HttpStatus status) -> _Response & {
  status_ = status;

  return *this;
}

/// @brief
/// @param client
void _Response::evaluateHeaders(ClientType &client) {
  if (body_ && body_ != F(""))
    headers[ContentLength] = body_.length();

  if (app.settings.count(XPoweredBy) > 0)
    headers[XPoweredBy] = app.settings[XPoweredBy];

  headers[F("connection")] = F("close");
}

/// @brief
/// @param client
void _Response::sendBody(ClientType &client, locals_t &locals) {
  LOG_V(F("sendBody"));

  // if we already have a body, send that over
  if (body_ && body_ != F(""))
    client.println(body_.c_str());
  else if (contentsCallback) {
    // a request to generate the body was issued earlier,
    // execute it here.
    int lastDot = filename.lastIndexOf('.');
    auto ext = filename.substring(lastDot + 1);

    LOG_V(F("file extension:"), ext);

    auto engineName = app.settings[F("view engine")];
    if (engineName.equals(ext)) {
      auto engine = app.engines[engineName];
      if (engine)
        engine(client, locals, options, contentsCallback());
    } else {
      LOG_V(F("using default renderer"));
      renderFile(client, options,
                 contentsCallback() /* callback */); // default renderer
    }
  }
}

/// @brief
void _Response::send() {
  auto &client = const_cast<ClientType &>(client_);

  client.print(F("HTTP/1.1 "));
  client.println(status_);

  // Construct headers
  evaluateHeaders(client);

  LOG_V(F("Headers:"));
  for (auto [first, second] : headers)
    LOG_V(first, second);

  // Send headers
  for (auto [first, second] : headers) {
    client.print(first);
    client.print(": ");
    client.println(second);
  }
  client.println();

  headersSent = true;

  sendBody(client, renderLocals);
}

END_EXPRESS_NAMESPACE
