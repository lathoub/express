#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class bodyParser
{
private:
    // TODO: static options
    // inflate, limit, reviver, strict, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static bool parseJson(Request &req, Response &res)
    {
        EX_DBG_I(F("bodyparser parseJson"));

        if (req.get(F("content-type")).equalsIgnoreCase(F("application/json")))
        {
            if (nullptr == req.stream)
            {
                EX_DBG_I(F("req.stream is null"));
                return true;
            }

            auto max_length = req.get(F("content-length")).toInt();

            req.body.reserve(max_length);

            if (!req.body.reserve(max_length + 1))
                return false;

            req.body[0] = 0;

            while (req.body.length() < max_length)
            {
                int tries = 1000;
                size_t avail;

                while (!((avail = req.stream->available())) && tries--)
                    delay(1);

                if (!avail)
                    break;

                if (req.body.length() + avail > max_length)
                    avail = max_length - req.body.length();

                while (avail--)
                    req.body += static_cast<char>(req.stream->read());
            }

            if (req.dataCallback_)
            {
                EX_DBG_I(F("calling event on data"));
                req.dataCallback_(nullptr);
            }

            res.headers_["content-type"] = F("application/json");

            if (req.endCallback_)
            {
                EX_DBG_I(F("calling event on end"));
                req.endCallback_();
            }

            return true;
        }

        return true;
    }

    // TODO: static options
    // inflate, limit, type, verify

    /// @brief defayults to application/octet-stream
    /// @param req
    /// @param res
    /// @return
    static bool parseRaw(Request &req, Response &res)
    {
        return true;
    }

    // TODO: static options
    // defaultCharset, inflate, limit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static bool parseText(Request &req, Response &res)
    {
        return true;
    }

    // TODO: static options
    // extended, inflate, limit, parameterLimit, type, verify

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static bool parseUrlencoded(Request &req, Response &res)
    {
        return true;
    }

public:
    /// @brief
    /// @param req
    /// @param res
    /// @return
    static MiddlewareCallback raw()
    {
        return bodyParser::parseRaw;
    }

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static MiddlewareCallback json()
    {
        return bodyParser::parseJson;
    }

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static MiddlewareCallback text()
    {
        return bodyParser::parseText;
    }

    /// @brief
    /// @param req
    /// @param res
    /// @return
    static MiddlewareCallback urlencoded()
    {
        return bodyParser::parseUrlencoded;
    }
};

END_EXPRESS_NAMESPACE
