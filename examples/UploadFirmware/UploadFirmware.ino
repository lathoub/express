#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_DEFAULT_INSTANCE();

int contentLength = 0;

auto getContentLength(request &req, response &res) -> bool {
  contentLength = req.headers[F("Content-Length")].toInt();
  return true;
}

void setup() {
  LOG_SETUP();

  Ethernet.init(5);
  Ethernet.begin(mac);

  // 2 middleware handlers, these will be executed in the same order as they are defined.
  // so: getContentLength before express::raw(). This way you can get the ContentLength
  // and use that in the events handlers 'data' and 'end' (eg to show % done).
  const HandlerCallback handlers[] = { getContentLength, express::raw() };

  route &route = app.post("/firmware", handlers, [](request &req, response &res) {
    LOG_V(F("all done"));
    res.sendStatus(HttpStatus::ACCEPTED);
  });

  route.on(F("data"), [](const Buffer &chunck) {
    LOG_V(F("data"), contentLength, F("chunck len:"), chunck.length);
  });

  route.on(F("end"), []() {
    LOG_V(F("end"));
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
