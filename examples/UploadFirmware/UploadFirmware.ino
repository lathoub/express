#define EX_DEBUG_LOGLEVEL EX_DEBUG_LOGLEVEL_VERBOSE
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

express app;

bool downloader(Request &req, Response &res) {
  EX_DBG_I(F("downloader middleware"));
  if (req.get(F("content-type")).equalsIgnoreCase(F("application/octet-stream"))) {
    req.on(F("data"), [](int) {
      EX_DBG_I(F("data"));
    });
    req.on(F("end"), []() {
      EX_DBG_I(F("end"));
    });
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {}

  Ethernet.init(5);
  Ethernet.begin(mac);

  app.post("/firmware", downloader, [](Request &req, Response &res) {
    res.send("lets see");
  });

  app.listen(80, []() {
    Serial.print(F("Example app listening on port "));
    Serial.print(Ethernet.localIP());
    Serial.print(F(" "));
    Serial.println(app.port);
  });
}

void loop() {
  app.run();
}
