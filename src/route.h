#pragma once

class Route
{
public:
    Method method = Method::UNDEFINED;
    String path{};
    MiddlewareCallback fptrMiddleware = nullptr;
    requestCallback fptrCallback = nullptr;
    // cache path splitting (avoid doing this for every request * number of paths)
    std::vector<PosLen> indices{};
};
