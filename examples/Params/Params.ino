#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_DEFAULT_INSTANCE();

void setup() {
  LOG_SETUP();

  Ethernet.begin(mac);

  app.get(F("/"), [](request &req, response &res) {
    res.send(F("Visit /user/0"));
  });

  app.get(F("/user/:user"), [](request &req, response &res) {
    res.send("user " + req.params["user"]);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
