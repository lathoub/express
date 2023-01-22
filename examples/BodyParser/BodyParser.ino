#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE


#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

express app;

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

 // app.use(express::json());

  app.post("/", express::json(), [](request& req, response& res) {
    res.send(req.body);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}