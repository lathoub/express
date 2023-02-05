#pragma once

#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

template<class, class, class> 
class Express;

using ContentCallback = const char *(*)();
using WriteCallback = void(*)(const char*, int);

struct File
{
    String filename;
    ContentCallback contentsCallback;
};

template <class T = int, class U = int, class Settings = DefaultSettings>
class Response
{
private:
    static void renderFile(EthernetClient &client, const char *f)
    {
        size_t i = 0;
        size_t start = 0;
        while (f[start + i] != '\0')
        {
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

public:
    String body_{};

    /// @brief
    const EthernetClient &client_;

    uint16_t status_ = HttpStatus::NOT_FOUND;

    dictionary<String, String, Settings::MaxHeaders> headers_{};

    /// @brief This property holds a reference to the instance of the Express application that is using the middleware.
    /// @return
    Express<T, U, Settings> &app_;

    /// @brief derefered rendering
    ContentCallback contentsCallback_{};
    locals_t renderLocals_{};
    String filename_;

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
            int lastDot = filename_.lastIndexOf('.');
            auto ext = filename_.substring(lastDot + 1);

            auto engineName = app_.settings[F("view engine")];
            if (engineName.equals(ext))
            {
                auto engine = app_.engines[engineName];
                if (engine)
                    engine(client, locals, contentsCallback_());
            }
            else
                renderFile(client, contentsCallback_()); // default renderer
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

public: /* Methods*/
    /// @brief Constructor
    Response(Express<T, U, Settings> &app, EthernetClient &client)
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

    /// @brief Sends the HTTP response.
    /// Optional parameters:
    /// @param view
    auto send(const String &body) -> void
    {
        body_ = body;
    }

    /// @brief Renders a view and sends the rendered HTML string to the client.
    /// Optional parameters:
    ///    - locals, an object whose properties define local variables for the view.
    ///    - callback, a callback function. If provided, the method returns both the
    ///      possible error and rendered string, but does not perform an automated response.
    ///      When an error occurs, the method invokes next(err) internally.
    /// @param view
    auto render(File &file, locals_t &locals) -> void
    {
        // NOTE: don't render here just yet (status and headers need to be prior prior)
        // so store a backpointer that can be called in the sendBody function.
        // set this here already, so it gets send out as part of the headers

        contentsCallback_ = file.contentsCallback;
        renderLocals_ = locals; // TODO: check if this copies??
        filename_ = file.filename;

        set(ContentType, F("text/html"));
    }

    /// @brief .
    auto sendFile(File &file, Options *options = nullptr) -> void
    {
        if (options)
        {
            for (auto [first, second] : options->headers)
            {
                LOG_V(first, second);
                set(first, second);
            }
        }

        contentsCallback_ = file.contentsCallback;
        filename_ = file.filename;
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

END_EXPRESS_NAMESPACE
