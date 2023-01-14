#define EX_DEBUG_LOGLEVEL EX_DEBUG_LOGLEVEL_VERBOSE
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

express app;

void setup() {
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {
  }
  delay(500);
  Serial.println(F("booting"));

  Ethernet.init(5);
  Ethernet.begin(mac);

  Route& route = app.post("/firmware", express::raw(), [](Request &req, Response &res) {
    res.send("lets see");
  });

  route.on(F("data"), [](const Buffer &chunck) {
    Serial.print(F("chunck size: "));
    Serial.println(chunck.length);
  });

  route.on(F("end"), []() {
    Serial.println(F("end"));
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
