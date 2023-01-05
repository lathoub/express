#include <Ethernet.h>
#include <Express.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetServer server(80);
EXPRESS_NAMESPACE::Express express;

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

  express.get("/helloworld", [](EXPRESS_NAMESPACE::HttpRequest &req, EXPRESS_NAMESPACE::HttpResponse &res) {
    EX_DBG(req.uri);
    res.status = 204;
  });

  express.get("/helloworld/:name", [](EXPRESS_NAMESPACE::HttpRequest &req, EXPRESS_NAMESPACE::HttpResponse &res) {
    EX_DBG(req.uri);
    EX_DBG(req.params["name"]);
    res.status = 204;
  });

  express.get("/", [](EXPRESS_NAMESPACE::HttpRequest &req, EXPRESS_NAMESPACE::HttpResponse &res) {
    res.body = "<!doctype html><meta charset=utf-8><title>shortest html5</title>";
    EX_DBG(req.uri);
    res.status = 200;
  });

  express.post("/firmware", [](EXPRESS_NAMESPACE::HttpRequest &req, EXPRESS_NAMESPACE::HttpResponse &res) {
    EX_DBG(req.uri);
    res.status = 201;
  });

  server.begin();
  Serial.print("Webserver listening on port ");
  Serial.println(80);
}

void loop() {
  if (EthernetClient client = server.available())
    express.run(client);
}