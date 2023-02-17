//#define LOGGER Serial
//#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

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
