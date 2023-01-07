#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

Express express;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  delay(1500);
  Serial.println("booting...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed DHCP, check network cable & reboot"));
    for (;;);
  }

  express.get("/helloworld", [](Request &req, Response &res) {
    res.status(204);
  });

  express.get("/helloworld/:name", [](Request &req, Response &res) {
    Serial.println(req.params["name"]);
    res.status(204);
  });

  express.get("/", [](Request &req, Response &res) {
    //   res.body = "<!doctype html><meta charset=utf-8><title>shortest html5</title>";
    // res.headers["content-type"] = "text/html;charset=utf-8";
    res.render("");
    res.status(200);
  });

  express.post("/firmware", [](Request &req, Response &res) {
    Serial.println(req.body);
    res.status(201);
  });

  express.listen(80, []() {
    EX_DBG("Webserver on IP:", Ethernet.localIP(), "listening on port:", express.port);
  });
}

void loop() {
  express.run();
}
