#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;
#include <basicAuth.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

  const std::map<String, String> users = { { F("admin"), F("supersecret123") } };
  app.use(basicAuth(users));

  app.get(F("/"), [](request &req, response &res) {
    res.send(F("Hello World!"));
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
