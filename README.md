# Express
Fast, unopinionated, minimalist web framework for Arduino

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
  
  express.get("/", [](HttpRequest &req, HttpResponse &res) {
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
