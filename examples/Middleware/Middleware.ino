#define DEBUG Serial

#include <Ethernet.h>
#include <Express.h>
using namespace EXPRESS_NAMESPACE;  // avoids adding namespace for each Express call

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetServer server(80);
Express express;

bool encoding1(HttpRequest &req, HttpResponse &res) {
  EX_DBG("step1");
  return false;
}

bool encoding2(HttpRequest &req, HttpResponse &res) {
  EX_DBG("step2");
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  delay(1500);
  Serial.println("booting...");

  Ethernet.init(5);

  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed DHCP, check network cable & reboot"));
    for (;;)
      ;
  }
  Serial.print("IP address is ");
  Serial.println(Ethernet.localIP());

  express.use(encoding1);  // middleware
  express.use(encoding2);  // middleware

  express.get("/", [](HttpRequest &req, HttpResponse &res) {
  EX_DBG("step3");
    res.status = 204;
  });

  server.begin();
  Serial.print("Webserver listening on port ");
  Serial.println(80);
}

void loop() {
  if (EthernetClient client = server.available())
    express.run(client);
}
