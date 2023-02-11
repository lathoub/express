#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();

bool middleware1(request &req, response &res) {
  req.params[F("msg")] = "first here";
  return true;
}

bool middleware2(request &req, response &res) {
  req.params[F("msg")] = req.params[F("msg")] + ", then here";
  return true;
}

bool middleware3(request &req, response &res) {
  req.params[F("msg")] = req.params[F("msg")] + ", and finally here.";
  return true;
}

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

  // add a single middleware
  app.use(middleware1);

  // or add an array of middlewares
  const std::vector<MiddlewareCallback> middlewares = { middleware2, middleware3 };
  app.use(middlewares);

  // the middleware will construct the message in the params
  app.get("/", [](request &req, response &res) {
    res.status(HttpStatus::OK).send(req.params[F("msg")]);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
