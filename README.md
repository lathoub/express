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
  app.run();
}
```

## Dependencies
Easy, none. The Ethernet libraries come preinstalled with the Arduino IDE.

### ESP32 with W5500 
When you combine an ESP32 with the W5500 chip, you need to patch Server.h as reported [here](https://github.com/PaulStoffregen/Ethernet/issues/42).

this `virtual void begin(uint16_t port=0) =0;` must be changed into `virtual void begin() =0;` 

- MacOS:   /Users/`user`/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
- Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
              
