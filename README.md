# Express
Fast, unopinionated, (very) minimalist web framework for Arduino

## Getting started

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
