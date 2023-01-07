# Express
Fast, unopinionated, (very) minimalist web framework for Arduino

## Hello World

```cpp
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express express;

void setup() {
  Serial.begin(115200);
  while (!Serial) { }  delay(1500);

  Ethernet.begin(mac); // no check for brevity

  express.get("/", [](Request &req, Response &res) {
    res.status(200).json("{'value': 42}");
  });

  express.listen(80, []() {
    Serial.print("Webserver listening on port:");
    Serial.println(express.port);
  });
}

void loop() {
  express.run();
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
