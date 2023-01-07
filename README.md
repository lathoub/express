# Express
Fast, unopinionated, (very) minimalist web framework for Arduino

## Hello World

```cpp
#include <Ethernet.h>
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetServer server(80);
Express express;

void setup() {
  Ethernet.begin(mac); // checks not included for brevity
  
  express.get("/", [](HttpRequest &req, Response &res) {
    res.body = "<!doctype html><meta charset=utf-8><title>Arduino Express</title>Hello World";
    res.headers["content-type"] = "text/html;charset=utf-8";
    res.status = 200;
  });

  server.begin();
}

void loop() {
  if (EthernetClient client = server.available())
    express.run(client);
}
```

## Routing Parameters

```cpp
express.get("/collections/:collectionId", [](HttpRequest &req, Response &res) {
    // req.params["collectionId"]
    res.status = 200;
  });
```

## http methods

```cpp
  express.get("/blabla", [](HttpRequest &req, Response &res) {
    res.status = 200;
  });
  
  express.put("/blabla", [](HttpRequest &req, Response &res) {
    // body in req.body
    res.status = 200;
  });

```

## Middleware(s)
```cpp
bool middleware1(HttpRequest &req, HttpResponse &res) {
  req.headers["aa"] = "aa";
  return true; 
}
...
  express.use("/v1"); // prefix all paths with /v1
  express.use(middleware1);

```
