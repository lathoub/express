#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

  app.get("/", [](request &req, response &res) {
    res.send("Got a GET request");
  });

  app.post("/", [](request &req, response &res) {
    res.send("Got a POST request");
  });

  app.put("/user", [](request &req, response &res) {
    res.send("Got a PUT request at /user");
  });

  app.Delete("/user", [](request &req, response &res) {
    res.send("Got a DELETE request at /user");
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
