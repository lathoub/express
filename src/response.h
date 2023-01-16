#pragma once

#include "mustache.h"

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

    FileCallback fileCallback_{};
    RenderCallback renderCallback_{};

private:
    /// @brief
    /// @param client
    void evaluateHeaders(EthernetClient &client)
    {
        if (body_ && !body_.isEmpty())
            headers_[F("content-length")] = body_.length();
        else
        {
            // TODO
        }
        headers_[F("connection")] = F("close");
    }

    /// @brief
    /// @param client
    void sendBody(EthernetClient &client, locals_t &locals)
    {
        EX_DBG_I(F("sendBody"));

        // send content length *or* close the connection (spec 7.2.2)
        if (body_ && !body_.isEmpty())
            client.println(body_.c_str());
        else if (fileCallback_)
            render_(client, locals, fileCallback_());
    }

    /// @brief
    /// @param client
    /// @param locals
    void render_(EthernetClient &client, locals_t &locals, const char *f)
    {
        unsigned int i = 0;
        unsigned int  start = 0;
        while (f[start + i] != '\0')
        {
            while (f[start + i] != '\n' && f[start + i] != '\0')
                i++;

            //     renderLine_(f, start, start + i, locals);

            if (f[start + i] == '\0')
                break;

            start += i + 1;
            i = 0;
        }
    }

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

        set(F("content-type"), F("application/json"));
        // QUESTION: set content-length here?
    }

    int find(const char *zin, const char *ss, const int van, const int tot)
    {
        int w = 0;
        int j = 0;
        for (auto i = van; i < tot; i++)
        {
            if (zin[i] == ss[j])
            {
                j++;
                w = i;
            }
            else if (w > 0)
                break; // optimization: stop after equality is found
        }

        return (2 == j) ? w : -1;
    }

    void renderLine_(const char *zin, int van, const int tot, locals_t &locals)
    {
        while (van < tot)
        {
            auto index = find(zin, "{{", van, tot);
            if (index < 1)
                break;

            // for (auto i = van; i < index - 1; i++)
            //     std::cout << zin[i];

            van = index + 1;

            index = find(zin, "}}", van, tot);
            if (index < 1)
                return; // TODO do error handling (no closing found), error in template

            String key(zin + van, (index - van - 1));
            // std::cout << locals[key];

            van = index + 1;
        }

        if (van < tot)
        { // remainder
          // for (auto i = van; i < tot; i++)
          //    std::cout << zin[i];
        }
    }

    /// @brief Renders a view and sends the rendered HTML string to the client.
    /// Optional parameters:
    ///    - locals, an object whose properties define local variables for the view.
    ///    - callback, a callback function. If provided, the method returns both the
    ///      possible error and rendered string, but does not perform an automated response.
    ///      When an error occurs, the method invokes next(err) internally.
    /// @param view
    auto render(FileCallback fileCallback, locals_t &locals) -> void
    {
        //  app_.engines;

        // TODO: don't render here just yet (status and headers need to be prior prior)
        // so store a backpointer that can be called in the sendBody function.
        // set this here already, so it gets send out as part of the headers

        fileCallback_ = fileCallback;

        set(F("content-type"), F("text/html"));
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

END_EXPRESS_NAMESPACE
