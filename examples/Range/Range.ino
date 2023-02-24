#include "setup.h"

EXPRESS_CREATE_INSTANCE();

class index {
public:
  static constexpr char *filename = "index.txt";
  static const char *content() {
    //      0123456789012345678901234567890123456789012345678901234 // 55 bytes total
    return "0aa3ab6bbbcc!ccdddddeeee@fffffggggghhhhhiiiiijjjjjkkkkk";
  }
};

void setup() {
  LOG_SETUP();

  ethernet_setup();

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    auto range = req.range(20); // clamp to 20 cars

    File file{index::filename, index::content};

    Options options;
    options.acceptRanges = true;
    options.headers[F("range")] = range.toString();

    res.sendFile(file, &options);
  });

  app.listen(80, []() { LOG_I(F("Example app listening on port"), app.port); });
}

void loop() { app.run(); }