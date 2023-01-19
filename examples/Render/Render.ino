#define EX_DEBUG
#define EX_DEBUG_LOGLEVEL EX_DEBUG_LOGLEVEL_VERBOSE

#include <Express.h>
#include <mustache.h>
using namespace EXPRESS_NAMESPACE;

#include "index.h";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

express app;

void setup() {
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {}

  Ethernet.init(5);
  Ethernet.begin(mac);

  // Register '.mustache' extension with The Mustache Express
  app.engine(F("mustache"), mustacheExpress());

  app.set(F("view engine"), F("mustache"));
  app.set(F("views"), __dirname + F("/views"));

  app.get("/", [](request &req, response &res) {
    //  if (false)
    //  res.json("{'hello': 'world'}");
    //else
    res.render(index::file, app.locals);
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
