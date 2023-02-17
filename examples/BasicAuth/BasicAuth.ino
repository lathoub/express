#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;
#include <basicAuth.h>

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

void setup() {
  LOG_SETUP();

  ethernet_setup();

  const std::map<String, String> users = {{F("admin"), F("supersecret123")}};
  app.use(basicAuth(users));

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    res.send(F("Hello World!"));
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"),
          app.port);
  });
}

void loop() { app.run(); }
