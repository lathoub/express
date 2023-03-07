![68747470733a2f2f692e636c6f756475702e636f6d2f7a6659366c4c376546612d3330303078333030302e706e67 (1)](https://user-images.githubusercontent.com/4082369/221396875-3191a433-a5cf-4174-9188-02c06efdd0be.png)

# express for ESP32 using Arduino IDE
Fast, unopinionated, (subset of a) minimalist web framework for ESP32 Arduino.

*looking for help* I'm looking for Arduino drop-in replacements for std::map and std::vector (understanding the memory contraints and possible issues in a ltd memory environment).

## Getting started

### Hello world example

Below is essentially the simplest Arduino express app you can create. 

```cpp
...
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

EXPRESS_CREATE_INSTANCE();

void setup() {
  ...

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
Ethernet library (for ESP32 with W5500).

## Use of Containers
This lib uses 2 types of containers: vector and map and work fine in the ESP32 environment. Not do much so in the Arduino environment and I'm looking for drop-in replacements. Anyone?

## ESP32 with W5500 
When you combine an ESP32 with the W5500 chip, you need to patch Server.h as reported [here](https://github.com/PaulStoffregen/Ethernet/issues/42).

this `virtual void begin(uint16_t port=0) =0;` must be changed into `virtual void begin() =0;` 

- MacOS:   /Users/`user`/Library/Arduino15/packages/esp32/hardware/esp32/2.0.*/cores/esp32
- Windows: C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.*\cores\esp32\Server.h
              
