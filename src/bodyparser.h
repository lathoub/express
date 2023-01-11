#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class bodyParser
{
private:
    /// @brief 
    /// @param req 
    /// @param res 
    /// @return 
    static bool parseJson(Request &req, Response &res)
    {
        if (req.get(F("content-type")).equalsIgnoreCase(F("application/json")))
        {
            if (nullptr == req.stream)
                return true;

            auto max_length = req.get(F("content-length")).toInt();

            EX_DBG_I(F("content-length:"), max_length);

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

            res.headers_["content-type"] = F("application/json");

            return true;
        }

        return true;
    }

    /// @brief defayults to application/octet-stream
    /// @param req 
    /// @param res 
    /// @return 
    static bool parseRaw(Request &req, Response &res)
    {
        return true;
    }

    /// @brief 
    /// @param req 
    /// @param res 
    /// @return 
    static bool parseText(Request &req, Response &res)
    {
        return true;
    }

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
