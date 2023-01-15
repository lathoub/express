#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class express;
class Routes;
class Route;
class bodyParser;

class Response
{
    friend class express;
    friend class Routes;
    friend class Route;
    friend class bodyParser;

private:
    String body_{};

    uint16_t status_ = HTTP_STATUS_NOT_FOUND;

    headers_t headers_{};

    /// @brief This property holds a reference to the instance of the Express application that is using the middleware.
    /// @return
    const express &app_;

public: /* Methods*/
    /// @brief Constructor
    Response(const express &express)
        : app_(express)
    {
    }

    /// @brief Appends the specified value to the HTTP response header field. If the header
    /// is not already set, it creates the header with the specified value. The value
    /// parameter can be a string or an array.
    /// Note: calling res.set() after res.append() will reset the previously-set header value.
    /// @param field
    /// @param value
    /// @return
    auto append(const String& field, const String& value) -> Response&
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

    /// @brief Ends the response process. This method actually comes from Node core,
    /// specifically the response.end() method of http.ServerResponse.
    /// @param data
    /// @param encoding
    /// @return
    auto end(const String& data, const String& encoding) -> Response&
    {
        return *this;
    }

    /// @brief Ends the response process
    static void end()
    {
    }

    /// @brief Returns the HTTP response header specified by field. The match is case-insensitive.
    /// @return
    auto get(const String& field) -> String
    {
        for (auto [key, header] : headers_)
        {
            if (field.equalsIgnoreCase(key))
                return header;
        }
        return "";
    }

    /// @brief Sends a JSON response. This method sends a response (with the correct content-type)
    /// that is the parameter converted to a JSON string using JSON.stringify().
    /// @param body
    /// @return
    auto json(const String& body) -> void
    {
        body_ = body;

        set(F("content-type"), F("application/json"));
        // QUESTION: set content-length here?

        return;
    }

    /// @brief Renders a view and sends the rendered HTML string to the client.
    /// Optional parameters:
    ///    - locals, an object whose properties define local variables for the view.
    ///    - callback, a callback function. If provided, the method returns both the
    ///      possible error and rendered string, but does not perform an automated response.
    ///      When an error occurs, the method invokes next(err) internally.
    /// @param view
    auto render(const String& view, locals_t& locals) -> void
    {
        // TODO
    }

    /// @brief Sends the HTTP response.
    /// Optional parameters:
    /// @param view
    auto send(const String& body) -> void
    {
        body_ = body;
    }

    /// @brief Sets the response HTTP status code to statusCode and sends the
    ///  registered status message as the text response body. If an unknown
    // status code is specified, the response body will just be the code number.
    /// @param statusCode
    auto sendStatus(const uint16_t statusCode) -> void
    {
        status_ = statusCode;
    }

    /// @brief Sets the response’s HTTP header field to value
    /// @param field
    /// @param value
    /// @return
    auto set(const String& field, const String& value) -> Response&
    {
        for (auto [key, header] : headers_)
        {
            if (field.equalsIgnoreCase(key))
            {
                // Appends the specified value to the HTTP response header
                header = value;
                return *this;
            }
        }

        // not found, creates the header with the specified value
        headers_[field] = value;

        return *this;
    }

    /// @brief Sends a JSON response. This method sends a response (with the correct content-type)
    /// that is the parameter converted to a JSON string using JSON.stringify().
    /// @param body
    /// @return
    auto status(const int status) -> Response&
    {
        status_ = status;

        return *this;
    }
};

END_EXPRESS_NAMESPACE
