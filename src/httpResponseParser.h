// Copied and modified from https://github.com/khoih-prog/EthernetWebServer/blob/master/src/Parsing-impl.h
// Copyright (c) 2019 Khoi Hoang under MIT license
// (now in public archieve (jan 2023))

#pragma once

#include <map>
#include "defs.h"

BEGIN_EXPRESS_NAMESPACE

class HttpRequestParser : public IHttpRequestParser
{
    static bool readBytesWithTimeout(EthernetClient &client, HttpRequest &req, int timeout_ms)
    {
        // TODO: use a download handler

        auto maxLength = req.contentLength;
        if (!req.body.reserve(maxLength + 1))
            return false;

        req.body[0] = 0;

        while (req.body.length() < maxLength)
        {
            int tries = timeout_ms;
            size_t avail;

            while (!(avail = client.available()) && tries--)
                delay(1);

            if (!avail)
                break;

            if (req.body.length() + avail > maxLength)
                avail = maxLength - req.body.length();

            while (avail--)
                req.body += (char)client.read();
        }

        return req.body.length() == maxLength;
    }

public:
    bool parseRequest(EthernetClient &client)
    {
        // Read the first line of HTTP request
        String reqStr = client.readStringUntil('\r');
        client.readStringUntil('\n');

        req.method = HttpMethod::UNDEFINED;
        req.version = "";
        req.uri = "";
        req.host = "";
        req.body = "";
        req.contentLength = 0;
        req.params.clear();
        req.headers.clear();
        req.arguments.clear();

        // First line of HTTP request looks like "GET /path HTTP/1.1"
        // Retrieve the "/path" part by finding the spaces
        auto addr_start = reqStr.indexOf(' ');
        auto addr_end = reqStr.indexOf(' ', addr_start + 1);

        if (addr_start == -1 || addr_end == -1)
        {
            EX_DBG(F("_parseRequest: Invalid request: "), reqStr);
            return false;
        }

        String methodStr = reqStr.substring(0, addr_start);
        String url = reqStr.substring(addr_start + 1, addr_end);
        String versionEnd = reqStr.substring(addr_end + 8);
        //  req.version = atoi(versionEnd.c_str());
        String searchStr = "";
        auto hasSearch = url.indexOf('?');

        if (hasSearch != -1)
        {
            searchStr = url.substring(hasSearch + 1);
            url = url.substring(0, hasSearch);
            EX_DBG("searchStr", searchStr);
        }

        req.uri = url;

        req.method = HttpMethod::GET;
        if (methodStr == "HEAD")
            req.method = HttpMethod::HEAD;
        else if (methodStr == "POST")
            req.method = HttpMethod::POST;
        else if (methodStr == "DELETE")
            req.method = HttpMethod::DELETE;
        else if (methodStr == "OPTIONS")
            req.method = HttpMethod::OPTIONS;
        else if (methodStr == "PUT")
            req.method = HttpMethod::PUT;
        else if (methodStr == "PATCH")
            req.method = HttpMethod::PATCH;

        // parse headers
        while (true)
        {
            reqStr = client.readStringUntil('\r');
            client.readStringUntil('\n');

            if (reqStr == "")
                break; // no more headers

            auto headerDiv = reqStr.indexOf(':');

            if (headerDiv == -1)
                break;

            auto headerName = reqStr.substring(0, headerDiv);
            headerName.toLowerCase();
            auto headerValue = reqStr.substring(headerDiv + 2);
            req.headers[headerName] = headerValue;

            if (headerName.equalsIgnoreCase(F("Host")))
                req.host = headerValue;
        }

        parseArguments(searchStr);

        bool isForm = false;
        bool isEncoded = false;
        String boundaryStr;
        if (req.method == HttpMethod::POST || req.method == HttpMethod::PUT || req.method == HttpMethod::PATCH || req.method == HttpMethod::DELETE)
        {
            for (auto [key, value] : req.headers)
            {
                if (key.equalsIgnoreCase("Content-Type"))
                {
                    if (value.startsWith("application/x-www-form-urlencoded"))
                    {
                        isForm = false;
                        isEncoded = true;
                    }
                    else if (value.startsWith("multipart/"))
                    {
                        boundaryStr = value.substring(value.indexOf('=') + 1);
                        boundaryStr.replace("\"", "");
                        isForm = true;
                    }
                }
                else if (key.equalsIgnoreCase("Content-Length"))
                {
                    req.contentLength = value.toInt();
                }
            }
        }

        if (!isForm)
        {
            EX_DBG(F("reading body:"));
            if (!readBytesWithTimeout(client, req, 3000))
            {
                EX_DBG(F("failed:"));
                return false;
            }
        }

        if (isEncoded)
        {
            // TODO
        }

        if (isForm)
        {
            EX_DBG(F("reading form:"));
            if (!parseForm(client, boundaryStr, req.contentLength))
            {
                EX_DBG(F("failed parseForm"));
                return false;
            }
        }

        client.flush();

        return true;
    }

    void parseArguments(const String &data)
    {
        if (data.length() == 0)
            return;

        int argCount = 1;

        for (int i = 0; i < static_cast<int>(data.length());)
        {
            i = data.indexOf('&', i);

            if (i == -1)
                break;

            ++i;
            ++argCount;
        }

        int pos = 0;

        for (int iarg = 0; iarg < argCount;)
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
            req.arguments[key] = value;

            ++iarg;

            if (next_arg_index == -1)
                break;

            pos = next_arg_index + 1;
        }
    }

    bool parseForm(EthernetClient &client, const String &boundary, uint32_t len)
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
                String argName;
                String argValue;
                String argType;
                String argFilename;

                bool argIsFile = false;

                line = client.readStringUntil('\r');
                client.readStringUntil('\n');

                if (line.length() > 19 && line.substring(0, 19).equalsIgnoreCase(F("Content-Disposition")))
                {
                    EX_DBG(F("line:"), line);

                    auto nameStart = line.indexOf('=');

                    if (nameStart != -1)
                    {
                        argName = line.substring(nameStart + 2);
                        nameStart = argName.indexOf('=');

                        if (nameStart == -1)
                        {
                            argName = argName.substring(0, argName.length() - 1);
                        }
                        else
                        {
                            argFilename = argName.substring(nameStart + 2, argName.length() - 1);
                            argName = argName.substring(0, argName.indexOf('"'));
                            argIsFile = true;

                            EX_DBG(F("PostArg FileName: "), argFilename);

                            // use GET to set the filename if uploading using blob
                            //    if (argFilename == F("blob") && hasArg("filename"))
                            //      argFilename = arg("filename");
                        }

                        line = client.readStringUntil('\r');
                        client.readStringUntil('\n');

                        if (line.length() > 12 && line.substring(0, 12).equalsIgnoreCase("Content-Type"))
                        {
                            argType = line.substring(line.indexOf(':') + 2);
                            // skip next line
                            client.readStringUntil('\r');
                            client.readStringUntil('\n');
                        }

                        if (!argIsFile)
                        {
                            while (true)
                            {
                                line = client.readStringUntil('\r');
                                client.readStringUntil('\n');

                                if (line.startsWith("--" + boundary))
                                    break;

                                if (argValue.length() > 0)
                                    argValue += "\n";

                                argValue += line;
                            }

                            // RequestArgument &arg = _postArgs[_postArgsLen++];
                            String key = argName;
                            String value = argValue;
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

    String urlDecode(const String &text)
    {
        String decoded = "";
        char temp[] = "0x00";
        unsigned int len = text.length();
        unsigned int i = 0;

        while (i < len)
        {
            char decodedChar;
            char encodedChar = text.charAt(i++);

            if ((encodedChar == '%') && (i + 1 < len))
            {
                temp[2] = text.charAt(i++);
                temp[3] = text.charAt(i++);

                decodedChar = strtol(temp, NULL, 16);
            }
            else
            {
                decodedChar = encodedChar == '+' ? ' ' : encodedChar;
            }

            decoded += decodedChar;
        }

        return decoded;
    }
};

END_EXPRESS_NAMESPACE