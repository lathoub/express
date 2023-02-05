#define LOGGER Serial
#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Override the default MIDI baudrate to
// a decoding program such as Hairless MIDI (set baudrate to 115200)
struct CustomSettings : public DefaultSettings {
  static constexpr int MaxRoutes = 25;
};

EXPRESS_CREATE_INSTANCE(app, EthernetServer, EthernetClient, CustomSettings);

void setup() {
  LOG_SETUP();

  Ethernet.begin(mac);

  app.get(F("/"), [](request &req, response &res) {
    res.send(F("Hello World!"));
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
