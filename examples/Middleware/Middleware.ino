#define DEBUG Serial

#include <Ethernet.h>
#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetServer server(80);
Express express;

bool middleware1(HttpRequest &req, HttpResponse &res) {
  req.headers["aa"] = "aa";
  EX_DBG("step1");
  return false; // don't go to next middle ware
}

bool middleware2(HttpRequest &req, HttpResponse &res) {
  req.headers["bbb"] = "bb";
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

  express.use("/v1");
  express.use(middleware1);
  express.use(middleware2);

  express.get("/", [](HttpRequest &req, HttpResponse &res) {
    EX_DBG("step3");
    EX_DBG(req.headers["aa"]);
    EX_DBG(req.headers["bb"]);
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
