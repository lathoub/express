#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

Buffer* favicon;

void setup() {
  LOG_SETUP();

  ethernet_setup();
  

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    res.send(F("Hello World!"));
  });

  favicon = Buffer::from("AAABAAEAEBAQAAAAAAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAA/4QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEREQAAAAAAEAAAEAAAAAEAAAABAAAAEAAAAAAQAAAQAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAEAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD//wAA//8AAP//AAD8HwAA++8AAPf3AADv+wAA7/sAAP//AAD//wAA+98AAP//AAD//wAA//8AAP//AAD//wAA", "base64");
  app.get(F("/favicon.ico"), [](request &req, response &res, const NextCallback next) {
      res.status(HttpStatus::OK);
      res.set("Content-Length", String(favicon->length));
      res.set("Content-Type", "image/x-icon");
//      res.set("Cache-Control", "public, max-age=2592000");
//      res.set("Expires", new Date(Date.now() + 2592000000).toUTCString());
      res.end(favicon);
    });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}
