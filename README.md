# Express for Arduino
Fast, unopinionated, (very) minimalist web framework for Arduino

## Getting started

### Hello world example

Embedded below is essentially the simplest Arduino Express app you can create. 

```cpp
#include <Express.h> // <Ethernet.h> included in Express.h
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express app;

void setup() {
  // ... setup for logging

  Ethernet.begin(mac); // no check for brevity

  app.get(F("/"), [](Request &req, Response &res) {
    res.send(F("Hello World!"));
  });

  app.listen(3000, []() { // creates and runs EthernetServer
    Serial.print(F("Example app listening on port "));
    Serial.println(express.port);
  });
}

void loop() {
  express.run();
}
```
