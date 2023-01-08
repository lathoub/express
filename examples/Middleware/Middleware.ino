#define DEBUG Serial

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

Express express;
Express v1;
Express v2;

bool middleware1(Request &req, Response &res) {
  return false;  // don't go to next middle ware
}

bool middleware2(Request &req, Response &res) {
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

  express.use("/v0");
  express.use("/v1", v1);
  express.use("/v2", v2);
  express.use(middleware1);
  express.use(middleware2);

  express.get("/hello", [](Request &req, Response &res) {
    EX_DBG(req.ip);
    res.status(HTTP_STATUS_OK).json("{'route': 'v0'}");
  });

  express.Delete("/hello", [](Request &req, Response &res) {
    EX_DBG(express.path());
    res.status(HTTP_STATUS_NO_CONTENT);
  });

  v1.get("/hello", [](Request &req, Response &res) {
    EX_DBG(v1.path());
    res.status(HTTP_STATUS_OK).json("{'route': 'v1'}");
  });

  v2.get("/hello", [](Request &req, Response &res) {
    res.status(HTTP_STATUS_OK).json("{'route': 'v2'}");
  });

  express.listen(80, []() {
    EX_DBG("Webserver on IP:", Ethernet.localIP(), "listening on port:", express.port);
  });
}

void loop() {
  express.run();
}
