// Copied and modified from https://github.com/khoih-prog/EthernetWebServer/blob/master/src/Parsing-impl.h
// Copyright (c) 2019 Khoi Hoang under MIT license
// (now in public archieve (jan 2023))

#pragma once

#include "defs.h"
#include "request.h"

BEGIN_EXPRESS_NAMESPACE

class HttpRequestParser
{
    Request req_;

public:
    /// @brief 
    /// @param client 
    /// @return 
    Request &parseRequest(EthernetClient &client) 
    {
        // Read the first line of HTTP request
        String reqStr = client.readStringUntil('\r');
        client.readStringUntil('\n');

        // TODO: clean
        req_.method = Method::UNDEFINED;
        req_.version_ = "";
        req_.uri_ = "";
        req_.hostname = "";
        req_.body = "";
        req_.params.clear();
        req_.headers.clear();
        req_.query.clear();

        req_.protocol = F("http");
        req_.secure = (req_.protocol == F("https"));
        req_.ip = client.remoteIP();

        // First line of HTTP request looks like "GET /path HTTP/1.1"
        // Retrieve the "/path" part by finding the spaces
        auto addr_start = reqStr.indexOf(' ');
        auto addr_end = reqStr.indexOf(' ', addr_start + 1);

        if (addr_start == -1 || addr_end == -1)
        {
            EX_DBG_V(F("_parseRequest: Invalid request: "), reqStr);
            req_.method = Method::ERROR;
            return req_;
        }

        auto method_str = reqStr.substring(0, addr_start);
        auto url = reqStr.substring(addr_start + 1, addr_end);
        auto version_end = reqStr.substring(addr_end + 8);
        //  req.version = atoi(versionEnd.c_str());
        String search_str = "";
        auto has_search = url.indexOf('?');

        if (has_search != -1) // TODO arguments or params ??
        {
            search_str = url.substring(has_search + 1);
            url = url.substring(0, has_search);
        }

        req_.uri_ = url;
        if (req_.uri_ == F("/")) req_.uri_ = F("");

        req_.method = Method::GET;
        if (method_str == F("HEAD"))
            req_.method = Method::HEAD;
        else if (method_str == "POST")
            req_.method = Method::POST;
        else if (method_str == "DELETE")
            req_.method = Method::DELETE;
        else if (method_str == "OPTIONS")
            req_.method = Method::OPTIONS;
        else if (method_str == "PUT")
            req_.method = Method::PUT;
        else if (method_str == "PATCH")
            req_.method = Method::PATCH;

        // parse headers
        while (true)
        {
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
            req_.headers[header_name] = header_value;

            if (header_name.equalsIgnoreCase(F("Host")))
                req_.hostname = header_value;
        }

        EX_DBG_V(F("Method:"), method_str);
        EX_DBG_V(F("Uri:"), req_.uri_);

        EX_DBG_V(F("Headers"));
        for (auto [header, value] : req_.headers)
            EX_DBG_V(F("header:"), header, F("value:"), value);

        parseArguments(search_str);

//        client.flush();

        return req_;
    }

    /// @brief
    /// @param data
    auto parseArguments(const String& data) -> void
    {
        if (data.length() == 0)
            return;

        int arg_count = 1;

        for (int i = 0; i < static_cast<int>(data.length());)
        {
            i = data.indexOf('&', i);

            if (i == -1)
                break;

            ++i;
            ++arg_count;
        }

        int pos = 0;

        for (int iarg = 0; iarg < arg_count;)
        {
            int equal_sign_index = data.indexOf('=', pos);
            int next_arg_index = data.indexOf('&', pos);

            if ((equal_sign_index == -1) || ((equal_sign_index > next_arg_index) && (next_arg_index != -1)))
            {
                if (next_arg_index == -1)
                    break;

                pos = next_arg_index + 1;
                continue;
            }

            String key = urlDecode(data.substring(pos, equal_sign_index));
            key.toLowerCase();
            String value = urlDecode(data.substring(equal_sign_index + 1, next_arg_index));
            req_.query[key] = value;

            ++iarg;

            if (next_arg_index == -1)
                break;

            pos = next_arg_index + 1;
        }

        EX_DBG_V(F("Query Arguments"));
        for (auto [argument, value] : req_.query)
            EX_DBG_V(F("argument:"), argument, F("value:"), value);
    }

    /// @brief 
    /// @param text 
    /// @return 
    auto urlDecode(const String& text) -> String
    {
        String decoded = "";
        char temp[] = "0x00";
        unsigned int len = text.length();
        unsigned int i = 0;

        while (i < len)
        {
            char decoded_char;
            char encoded_char = text.charAt(i++);

            if ((encoded_char == '%') && (i + 1 < len))
            {
                temp[2] = text.charAt(i++);
                temp[3] = text.charAt(i++);

                decoded_char = strtol(temp, NULL, 16);
            }
            else
            {
                decoded_char = encoded_char == '+' ? ' ' : encoded_char;
            }

            decoded += decoded_char;
        }

        return decoded;
    }
};

END_EXPRESS_NAMESPACE