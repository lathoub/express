//#define LOGGER Serial
//#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <Express.h>
using namespace EXPRESS_NAMESPACE;
#include <middlewares/Mustache.h>

#include "ethernet_setup.h"

EXPRESS_CREATE_INSTANCE();

// Content that will become a File
class index {
public:
  static constexpr char* filename = "index.mustache"; // with .mustache ext
  static const char* content() {
    return "<!doctype html><title>{{title}}</title>\n";
  }
};

void setup() {
  LOG_SETUP();

  ethernet_setup();
  
  // Register '.mustache' extension with The Mustache MUSTACHE
  app.engine(F("mustache"), mustacheEXPRESS());

  app.set("view engine", "mustache");
  app.set("views", __dirname + "/views");

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    locals_t locals;
    locals[F("title")] = F("hello world!");
    File file{ index::filename, index::content };
    res.render(file, locals);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}