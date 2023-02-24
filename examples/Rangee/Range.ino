#include "setup.h"

EXPRESS_CREATE_INSTANCE();

class index {
public:
  static constexpr char *filename = "index.txt";
  static const char *content() {
    return "*123456789!123456789|123456789>123456789<123456789=";
  }
};

void setup() {
  LOG_SETUP();

  ethernet_setup();

  app.get(F("/"), [](request &req, response &res, const NextCallback next) {
    auto range = req.range();
    LOG_V("range:", range.type, range.start, range.end);

    File file{index::filename, index::content};

    if (range.type.length() > 0) {
      if (range.end < 0)
        range.end = file.length() - 1; // no end was given, so take file length

      if ((range.start > file.length()) || (range.end < range.start)) {
        res.set(F("Content-Range"), String("bytes */") + file.length());
        res.status(HttpStatus::RANGE_NOT_SATISFIABLE);
        return;
      }

      Options options;
      options.acceptRanges = true;
      options.headers[F("range")] = range.toString();

      res.sendFile(file, &options);
      res.set(F("Content-Range"), range.toString() + F("/") + file.length());
      res.set(F("Content-Length"), String(range.end - range.start + 1));
      res.set(F("Accept-Ranges"), F("bytes"));
      res.status(HttpStatus::PARTIAL_CONTENT);
    } else {
      res.sendFile(file);
    }
  });

  app.listen(80, []() { LOG_I(F("Example app listening on port"), app.port); });
}

void loop() { app.run(); }