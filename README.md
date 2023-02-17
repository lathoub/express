# express for ESP32 using Arduino IDE
Fast, unopinionated, (subset of a) minimalist web framework for Arduino

## Getting started

### Hello world example

Embedded below is essentially the simplest Arduino express app you can create. 

```cpp
// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

void setup() {
  ethernet_setup();

  app.get(F("/"), [](request &req, response &res) {
    res.send(F("Hello World!"));
  });

  app.listen(3000, []() { // creates and runs server
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}
```

## Dependencies
None.

## Use of Containers
This lib uses 2 types of containers: vector and map.

## ESP32 with W5500 
When you combine an ESP32 with the W5500 chip, you need to patch Server.h as reported [here](https://github.com/PaulStoffregen/Ethernet/issues/42).

this `virtual void begin(uint16_t port=0) =0;` must be changed into `virtual void begin() =0;` 

- MacOS:   /Users/`user`/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
- Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
              
