#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

express app;

bool middleware1(Request &req, Response &res) {
  req.params[F("demo")]= "hello";
  return true;  
}

bool middleware2(Request &req, Response &res) {
  req.params[F("demo")]= req.params[F("demo")] + " world";
  return true; 
}

void setup() {
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {}

  Ethernet.begin(mac);

  app.use(middleware1);
  app.use(middleware2);

  app.get("/", [](Request &req, Response &res) {
    res.status(HTTP_STATUS_OK).send(req.params[F("demo")]);
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
