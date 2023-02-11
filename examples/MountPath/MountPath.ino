#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();
EXPRESS_CREATE_NAMED_INSTANCE(v1);
EXPRESS_CREATE_NAMED_INSTANCE(v2);

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

  app.use("/v1", v1);
  app.use("/v2", v2);

  app.get("/user", [](request &req, response &res, const NextCallback next) { 
    res.status(HttpStatus::OK).send("root path");
  });

  // Landing page here is /v1/user
  v1.get("/user", [](request &req, response &res, const NextCallback next) { 
    res.status(HttpStatus::OK).send("route v1");
  });

  // Landing page here is /v2/user
  v2.get("/user", [](request &req, response &res, const NextCallback next) { 
    res.status(HttpStatus::OK).send("route v2");
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
