//#define LOGGER Serial
//#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();

void setup() {
  LOG_SETUP();

  ethernet_setup();
  

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send(F("Hello World!"));
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}
