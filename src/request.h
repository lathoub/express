#pragma once
#include <map>

class Request
{
public:
    Method method;
    String version;
    String uri;
    String host;
    String body;
    int contentLength;
    std::map<String, String> params;
    std::map<String, String> headers;
    std::map<String, String> arguments;

public:
};
