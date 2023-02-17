#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

auto apiv2 = express::Router();
auto apiv3 = express::Router();
  
void setup() {
  LOG_SETUP();

  ethernet_setup();
  

  // inspired by
  // https://github.com/expressjs/express/blob/master/examples/multi-router/index.js

  app.use("/");
  app.use("/api/v2", apiv2);
  app.use("/api/v3", apiv3);

  // /user will be mounted on /v1/user
  app.get("/", [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send("Hello from root route");
  });

  apiv2.get("/", [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send("Hello from APIv2 root route.");
  });

  apiv2.get("/users", [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send("List of APIv2 users.");
  });

  apiv3.get("/", [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send("Hello from APIv3 root route.");
  });

  apiv3.get("/users", [](request &req, response &res, const NextCallback next) {
    res.status(HttpStatus::OK).send("List of APIv3 users");
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"),
          app.port);
  });
}

void loop() { app.run(); }