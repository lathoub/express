// Copied and modified from https://github.com/khoih-prog/EthernetWebServer/blob/master/src/Parsing-impl.h
// Copyright (c) 2019 Khoi Hoang under MIT license
// (now in public archieve (jan 2023))

#pragma once

#include <map>
#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

class HttpRequestParser : public IHttpRequestParser
{
    Request req_;

    /// @brief 
    /// @param client 
    /// @param req 
    /// @param timeout_ms 
    /// @return 
    static bool bodyParser(EthernetClient &client, Request &req, const int timeout_ms)
    {
        // TODO: use a download handler

        auto max_length = req.contentLength_;
        if (!req.body.reserve(max_length + 1))
            return false;

        req.body[0] = 0;

        while (req.body.length() < max_length)
        {
            int tries = timeout_ms;
            size_t avail;

            while (!((avail = client.available())) && tries--)
                delay(1);

            if (!avail)
                break;

            if (req.body.length() + avail > max_length)
                avail = max_length - req.body.length();

            while (avail--)
                req.body += static_cast<char>(client.read());
        }

        return req.body.length() == max_length;
    }

public:
    /// @brief 
    /// @param client 
    /// @return 
    Request &parseRequest(EthernetClient &client) override
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
        req_.contentLength_ = 0;
        req_.params.clear();
        req_.headers_.clear();
        req_.query.clear();

        req_.protocol = "http";
        req_.secure = (req_.protocol == "https");
        req_.ip = client.remoteIP();

        // First line of HTTP request looks like "GET /path HTTP/1.1"
        // Retrieve the "/path" part by finding the spaces
        auto addr_start = reqStr.indexOf(' ');
        auto addr_end = reqStr.indexOf(' ', addr_start + 1);

        if (addr_start == -1 || addr_end == -1)
        {
            EX_DBG(F("_parseRequest: Invalid request: "), reqStr);
            req_.method = Method::ERROR;
            return req_;
        }

        String method_str = reqStr.substring(0, addr_start);
        String url = reqStr.substring(addr_start + 1, addr_end);
        String version_end = reqStr.substring(addr_end + 8);
        //  req.version = atoi(versionEnd.c_str());
        String search_str = "";
        auto has_search = url.indexOf('?');

        if (has_search != -1) // TODO arguments or params ??
        {
            search_str = url.substring(has_search + 1);
            url = url.substring(0, has_search);
        }

        req_.uri_ = url;

        req_.method = Method::GET;
        if (method_str == "HEAD")
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
            req_.headers_[header_name] = header_value;

            if (header_name.equalsIgnoreCase(F("Host")))
                req_.hostname = header_value;
        }

        parseArguments(search_str);

        bool is_form = false;
        bool is_encoded = false;
        String boundary_str;
        if (req_.method == Method::POST || req_.method == Method::PUT || req_.method == Method::PATCH || req_.method == Method::DELETE)
        {
            for (auto [key, value] : req_.headers_)
            {
                if (key.equalsIgnoreCase("Content-Type"))
                {
                    if (value.startsWith("application/x-www-form-urlencoded"))
                    {
                        is_form = false;
                        is_encoded = true;
                    }
                    else if (value.startsWith("multipart/"))
                    {
                        boundary_str = value.substring(value.indexOf('=') + 1);
                        boundary_str.replace("\"", "");
                        is_form = true;
                    }
                }
                else if (key.equalsIgnoreCase("Content-Length"))
                {
                    req_.contentLength_ = value.toInt();
                }
            }
        }

        if (!is_form)
        {
            if (!bodyParser(client, req_, 3000)) // TODO: can be overwritten
            {
                EX_DBG(F("failed:"));
                req_.method = Method::ERROR;
                return req_;
            }
        }

        if (is_encoded)
        {
            // TODO
        }

        if (is_form)
        {
            EX_DBG(F("reading form:"));
            if (!parseForm(client, boundary_str, req_.contentLength_))
            {
                EX_DBG(F("failed parseForm"));
                req_.method = Method::ERROR;
                return req_;
            }
        }

        client.flush();

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
    }

    /// @brief 
    /// @param client 
    /// @param boundary 
    /// @param len 
    /// @return 
    auto parseForm(EthernetClient& client, const String& boundary, uint32_t len) -> bool
    {
        EX_DBG(F("boundary"), boundary);
        EX_DBG(F("len"), len);

        String line;
        int retry = 0;

        do
        {
            line = client.readStringUntil('\r');
            ++retry;
        } while (line.length() == 0 && retry < 3);

        client.readStringUntil('\n');

        // start reading the form
        if (line == ("--" + boundary))
        {
            while (true)
            {
                String arg_name;
                String arg_value;
                String arg_type;
                String arg_filename;

                line = client.readStringUntil('\r');
                client.readStringUntil('\n');

                if (line.length() > 19 && line.substring(0, 19).equalsIgnoreCase(F("Content-Disposition")))
                {
                    EX_DBG(F("line:"), line);

                    auto name_start = line.indexOf('=');

                    if (name_start != -1)
                    {
	                    bool arg_is_file = false;
	                    arg_name = line.substring(name_start + 2);
                        name_start = arg_name.indexOf('=');

                        if (name_start == -1)
                        {
                            arg_name = arg_name.substring(0, arg_name.length() - 1);
                        }
                        else
                        {
                            arg_filename = arg_name.substring(name_start + 2, arg_name.length() - 1);
                            arg_name = arg_name.substring(0, arg_name.indexOf('"'));
                            arg_is_file = true;

                            EX_DBG(F("PostArg FileName: "), arg_filename);

                            // use GET to set the filename if uploading using blob
                            //    if (argFilename == F("blob") && hasArg("filename"))
                            //      argFilename = arg("filename");
                        }

                        line = client.readStringUntil('\r');
                        client.readStringUntil('\n');

                        if (line.length() > 12 && line.substring(0, 12).equalsIgnoreCase("Content-Type"))
                        {
                            arg_type = line.substring(line.indexOf(':') + 2);
                            // skip next line
                            client.readStringUntil('\r');
                            client.readStringUntil('\n');
                        }

                        if (!arg_is_file)
                        {
                            while (true)
                            {
                                line = client.readStringUntil('\r');
                                client.readStringUntil('\n');

                                if (line.startsWith("--" + boundary))
                                    break;

                                if (arg_value.length() > 0)
                                    arg_value += "\n";

                                arg_value += line;
                            }

                            // RequestArgument &arg = _postArgs[_postArgsLen++];
                            String key = arg_name;
                            String value = arg_value;
                            EX_DBG(F("key: "), key, F("value: "), value);

                            if (line == ("--" + boundary + "--"))
                            {
                                EX_DBG(F("Done Parsing POST"));
                                break;
                            }
                        }
                        else
                        {
                            //_currentUpload.reset(new ethernetHTTPUpload());
                            /*                            if (!_currentUpload)
                                                            _currentUpload = new ethernetHTTPUpload();

                                                        _currentUpload->status = UPLOAD_FILE_START;
                                                        _currentUpload->name = argName;
                                                        _currentUpload->filename = argFilename;
                                                        _currentUpload->type = argType;
                                                        _currentUpload->totalSize = 0;
                                                        _currentUpload->currentSize = 0;
                                                        _currentUpload->contentLength = len;

                                                        EX_DBG(F("Start File: "), _currentUpload->filename);
                                                        EX_DBG(F("Type: "), _currentUpload->type);

                                                        if (_currentHandler && _currentHandler->canUpload(_currentUri))
                                                            _currentHandler->upload(*this, _currentUri, *_currentUpload);

                                                        _currentUpload->status = UPLOAD_FILE_WRITE;
                                                        uint8_t argByte = _uploadReadByte(client);
                            */
                        readfile:
                            /*
                                                        while (argByte != 0x0D)
                                                        {
                                                            if (!client.connected())
                                                                return _parseFormUploadAborted();

                                                            _uploadWriteByte(argByte);
                                                            argByte = _uploadReadByte(client);
                                                        }

                                                        argByte = _uploadReadByte(client);

                                                        if (!client.connected())
                                                            return _parseFormUploadAborted();

                                                        if (argByte == 0x0A)
                                                        {
                                                            argByte = _uploadReadByte(client);

                                                            if (!client.connected())
                                                                return _parseFormUploadAborted();

                                                            if ((char)argByte != '-')
                                                            {
                                                                // continue reading the file
                                                                _uploadWriteByte(0x0D);
                                                                _uploadWriteByte(0x0A);
                                                                goto readfile;
                                                            }
                                                            else
                                                            {
                                                                argByte = _uploadReadByte(client);

                                                                if (!client.connected())
                                                                    return _parseFormUploadAborted();

                                                                if ((char)argByte != '-')
                                                                {
                                                                    // continue reading the file
                                                                    _uploadWriteByte(0x0D);
                                                                    _uploadWriteByte(0x0A);
                                                                    _uploadWriteByte((uint8_t)('-'));
                                                                    goto readfile;
                                                                }
                                                            }

                                                            // Better compiler warning than risk of fragmented heap
                                                            uint8_t endBuf[boundary.length()];

                                                            client.readBytes(endBuf, boundary.length());

                                                            if (strstr((const char *)endBuf, boundary.c_str()) != NULL)
                                                            {
                                                                if (_currentHandler && _currentHandler->canUpload(_currentUri))
                                                                    _currentHandler->upload(*this, _currentUri, *_currentUpload);

                                                                _currentUpload->totalSize += _currentUpload->currentSize;
                                                                _currentUpload->status = UPLOAD_FILE_END;

                                                                if (_currentHandler && _currentHandler->canUpload(_currentUri))
                                                                    _currentHandler->upload(*this, _currentUri, *_currentUpload);

                                                                EX_DBG(F("End File: "), _currentUpload->filename);
                                                                EX_DBG(F("Type: "), _currentUpload->type);
                                                                EX_DBG(F("Size: "), _currentUpload->totalSize);

                                                                line = client.readStringUntil(0x0D);
                                                                client.readStringUntil(0x0A);

                                                                if (line == "--")
                                                                {
                                                                    ET_LOGDEBUG(F("Done Parsing POST"));

                                                                    break;
                                                                }

                                                                continue;
                                                            }
                                                            else
                                                            {
                                                                _uploadWriteByte(0x0D);
                                                                _uploadWriteByte(0x0A);
                                                                _uploadWriteByte((uint8_t)('-'));
                                                                _uploadWriteByte((uint8_t)('-'));

                                                                uint32_t i = 0;

                                                                while (i < boundary.length())
                                                                {
                                                                    _uploadWriteByte(endBuf[i++]);
                                                                }

                                                                argByte = _uploadReadByte(client);
                                                                goto readfile;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            _uploadWriteByte(0x0D);
                                                            goto readfile;
                                                        }
                            */
                            break;
                        }
                    }
                }
            }

            int iarg;
            int totalArgs = 0; //((WEBSERVER_MAX_POST_ARGS - _postArgsLen) < _currentArgCount) ? (WEBSERVER_MAX_POST_ARGS - _postArgsLen) : _currentArgCount;

            for (iarg = 0; iarg < totalArgs; iarg++)
            {
                //               String key = _currentArgs[iarg].key;
                //             String value = _currentArgs[iarg].value;
                //               RequestArgument &arg = _postArgs[_postArgsLen++];
            }
            /*
                        _currentArgs = new RequestArgument[_postArgsLen];

                        if (_currentArgs == nullptr)
                        {
                            EX_DBG(F("EthernetWebServer::_parseForm: null _currentArgs"));

                            return false;
                        }

                        for (iarg = 0; iarg < _postArgsLen; iarg++)
                        {
                            RequestArgument &arg = _currentArgs[iarg];
                            arg.key = _postArgs[iarg].key;
                            arg.value = _postArgs[iarg].value;
                        }

                        _currentArgCount = iarg;

                        if (_postArgs)
                        {
                            delete[] _postArgs;
                            _postArgs = nullptr;
                            _postArgsLen = 0;
                        }
            */
            return true;
        }
        return true;
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