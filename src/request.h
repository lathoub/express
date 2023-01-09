#pragma once

#include <map>

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class Express;
class Routes;
class Route;

class Request
{
    friend class Express;
    friend class Routes;
    friend class Route;
    friend class HttpRequestParser;

private:
    String version_;

    String uri_;

    int contentLength_;

public:

    /// @brief This property holds a reference to the instance of the Express application that is using the middleware.
    /// @return 
    Express* app = nullptr;

    /// @brief Contains a string corresponding to the HTTP method of the request: GET, POST, PUT, and so on.
    Method method;

    /// @brief Contains the hostname derived from the Host HTTP header
    String hostname;

    /// @brief A Boolean property that is true if a TLS connection is established.
    ///  Equivalent to: (req.protocol === 'https')
    bool secure;

    /// @brief
    String body;

    /// @brief Contains the remote IP address of the request.
    IPAddress ip;

    /// @brief 
    std::map<String, String> headers;

    /// @brief Contains the path part of the request URL.
    String path;

    /// @brief Contains the request protocol string: either http or (for TLS requests) https.
    String protocol;

    /// @brief 
    std::map<String, String> query;

    /// @brief This property is an object containing properties mapped to the named route “parameters”.
    /// For example, if you have the route /user/:name, then the “name” property is available as
    //  req.params[name]
    std::map<String, String> params;

public: /* Methods*/
    /// @brief Checks if the specified content types are acceptable, based on the request’s Accept HTTP
    /// header field. The method returns the best match, or if none of the specified content types is
    /// acceptable, returns false (in which case, the application should respond with 406 "Not Acceptable").
    bool accepts(const String& types)
    {
        return false;
    }

    /// @brief Returns the specified HTTP request header field (case-insensitive match). 
    /// @param field
    /// @return
    String get(const String& field)
    {
        for (auto [key, header] : headers)
        {
            if (field.equalsIgnoreCase(key))
                return header;
        }
        return "";
    }

};

END_EXPRESS_NAMESPACE
