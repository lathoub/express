#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

void middleware1(request &req, response &res, const NextCallback next) {
  req.params[F("msg")] = req.params[F("msg")] + ", first here";
  next(nullptr);
}

void middleware2(request &req, response &res, const NextCallback next) {
  req.params[F("msg")] = req.params[F("msg")] + ", then here";
  next(nullptr);
}

void middleware3(request &req, response &res, const NextCallback next) {
  req.params[F("msg")] = req.params[F("msg")] + ", and finally here.";
  next(nullptr);
}

void setup() {
  LOG_SETUP();

  ethernet_setup();

  // add a single middleware
  // app.use(middleware1);

  // or add an array of middlewares
  const std::vector<MiddlewareCallback> middlewares = {middleware2,
                                                       middleware3};
  // app.use(middlewares);

  // the middleware will construct the message in the params
  app.get(
      "/", middleware1, nullptr, middlewares,
      [](request &req, response &res, const NextCallback next) {
        LOG_I(F("hallo!"));
        next(nullptr);
      },
      [](request &req, response &res, const NextCallback next) {
        LOG_I(F("world!"));
        res.status(HttpStatus::OK).send(req.params[F("msg")]);
      });

  app.listen(80, []() { LOG_I(F("Example app listening on port"), app.port); });
}

void loop() { app.run(); }
