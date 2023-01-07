#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express express;

void setup() {
  Serial.begin(115200);
  while (!Serial) { }  delay(1500);
  Serial.println("booting...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed DHCP, check network cable & reboot")); for (;;);
  }

  express.get("/", [](Request &req, Response &res) {
    res.send("Hello World!"));
  });

  express.listen(3000, []() {
    Serial.print("Example app listening on port ");
    Serial.println(express.port);
  });
}

void loop() {
  express.run();
}
