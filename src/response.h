#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class Routes;
class Route;
class express;

using ContentCallback = const char *(*)();

struct ResponseDefaultSettings
{
    /// @brief
    static constexpr int MaxHeaders = 5;
};

template <class _Settings = ResponseDefaultSettings>
class Response
{
public:
    typedef _Settings Settings;

    String body_{};

    /// @brief
    const EthernetClient &client_;

    uint16_t status_ = HttpStatus::NOT_FOUND;

    dictionary<String, String, Settings::MaxHeaders> headers_{};

    /// @brief This property holds a reference to the instance of the Express application that is using the middleware.
    /// @return
    express &app_;

    /// @brief derefered rendering
    ContentCallback contentsCallback_{};
    locals_t renderLocals_{};

public:
    /// @brief
    /// @param client
    void evaluateHeaders(EthernetClient &client)
    {
        if (body_ && body_ != F(""))
            headers_[ContentLength] = body_.length();

        headers_[F("connection")] = F("close");
    }

    /// @brief
    /// @param client
    void sendBody(EthernetClient &client, locals_t &locals)
    {
        if (body_ && body_ != F(""))
            client.println(body_.c_str());
        else if (contentsCallback_)
        {
            auto engineName = app_.settings[F("view engine")];
            auto engine = app_.engines[engineName];
            if (engine)
                engine(client, locals, contentsCallback_());
        }
    }

    /// @brief
    void send()
    {
        auto &client = const_cast<EthernetClient &>(client_);

        client.print(F("HTTP/1.1 "));
        client.println(status_);

        // Add to headers
        evaluateHeaders(client);

        if (app_.settings.count(XPoweredBy) > 0)
            headers_[XPoweredBy] = app_.settings[XPoweredBy];

        // Send headers
        for (auto [first, second] : headers_)
        {
            client.print(first);
            client.print(": ");
            client.println(second);
        }
        client.println();

        sendBody(client, renderLocals_);

        client.setConnectionTimeout(5);
        client.stop();
    }

public: /* Methods*/
    /// @brief Constructor
    Response(express &app, EthernetClient &client)
        : app_(app), client_(client)
    {
    }

    /// @brief Appends the specified value to the HTTP response header field. If the header
    /// is not already set, it creates the header with the specified value. The value
    /// parameter can be a string or an array.
    /// Note: calling res.set() after res.append() will reset the previously-set header value.
    /// @param field
    /// @param value
    /// @return
    auto append(const String &field, const String &value) -> Response &
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
    auto end(const String &data, const String &encoding) -> Response &
    {
        return *this;
    }

    /// @brief Ends the response process
    static void end()
    {
    }

    /// @brief Returns the HTTP response header specified by field. The match is case-insensitive.
    /// @return
    auto get(const String &field) -> String
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
    auto json(const String &body) -> void
    {
        body_ = body;

        set(ContentType, ApplicationJson);
        // QUESTION: set content-length here?
    }

    /// @brief Renders a view and sends the rendered HTML string to the client.
    /// Optional parameters:
    ///    - locals, an object whose properties define local variables for the view.
    ///    - callback, a callback function. If provided, the method returns both the
    ///      possible error and rendered string, but does not perform an automated response.
    ///      When an error occurs, the method invokes next(err) internally.
    /// @param view
    auto render(ContentCallback fileCallback, locals_t &locals) -> void
    {
        // NOTE: don't render here just yet (status and headers need to be prior prior)
        // so store a backpointer that can be called in the sendBody function.
        // set this here already, so it gets send out as part of the headers

        contentsCallback_ = fileCallback;
        renderLocals_ = locals; // TODO: check if this copies??

        set(ContentType, F("text/html"));
    }

    /// @brief Sends the HTTP response.
    /// Optional parameters:
    /// @param view
    auto send(const String &body) -> void
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
    auto set(const String &field, const String &value) -> Response &
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
    auto status(const int status) -> Response &
    {
        status_ = status;

        return *this;
    }
};

typedef Response<> response;

END_EXPRESS_NAMESPACE
