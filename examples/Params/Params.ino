#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

void setup() {
  LOG_SETUP();

  ethernet_setup();
  

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    res.send(F("Visit /user/0"));
  });

  app.get(F("/user/:user"), [](request &req, response &res, const NextCallback next) {
    res.send("user " + req.params["user"]);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}
