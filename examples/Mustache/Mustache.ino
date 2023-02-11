//#define LOGGER Serial
//#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;
#include <Mustache.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

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

  Ethernet.init(5);
  Ethernet.begin(mac);

  // Register '.mustache' extension with The Mustache MUSTACHE
  app.engine(F("mustache"), mustacheEXPRESS());

  app.get(F("/"), [](request &req, response &res) {
    locals_t locals;
    locals
    File file{ index::filename, index::content };
    res.render(file, locals);
  });

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), Ethernet.localIP(), F("on port"), app.port);
  });
}

void loop() {
  app.run();
}
