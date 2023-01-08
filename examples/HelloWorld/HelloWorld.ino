#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express app;

void setup() {
  Serial.begin(115200);
  while (!Serial) { }  delay(1500);

  Ethernet.begin(mac);

  app.get(F("/"), [](Request &req, Response &res) {
    res.send(F("Hello World!"));
  });

  app.listen(3000, []() {
    Serial.print(F("Example app listening on port "));
    Serial.println(app.port);
  });
}

void loop() {
  app.run();
}
