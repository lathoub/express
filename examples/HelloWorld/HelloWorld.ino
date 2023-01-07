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
    res.status(200).json("{'value': 42}");
  });

  express.listen(80, []() {
    EX_DBG("Webserver on IP:", Ethernet.localIP(), "listening on port:", express.port);
  });
}

void loop() {
  express.run();
}
