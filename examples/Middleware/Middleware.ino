#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

express app;

bool middleware1(request &req, response &res) {
  req.params[F("demo")]= "hello";
  return true;  
}

bool middleware2(request &req, response &res) {
  req.params[F("demo")]= req.params[F("demo")] + " world";
  return true; 
}

void setup() {
  LOG_SETUP();

  Ethernet.begin(mac);

  app.use(middleware1);
  app.use(middleware2);

  app.get("/", [](request &req, response &res) {
    res.status(HttpStatus::OK).send(req.params[F("demo")]);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
