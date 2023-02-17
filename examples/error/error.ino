//#define LOGGER Serial
//#define LOG_LOGLEVEL LOG_LOGLEVEL_VERBOSE

// #define PLATFORM ESP32
#define PLATFORM ESP32_W5500

#include <express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EXPRESS_CREATE_INSTANCE();

// error handling middleware have an arity of 4
// instead of the typical (req, res, next),
// otherwise they behave exactly like regular
// middleware, you may have several of them,
// in different orders etc.
void errorLogger(Error& error, request & req, response & res, const NextCallback next) {
  // log it
  LOG_E(F("o-ho, an error occured"));
  next(&error);
}

void errorResponder(Error& err, request & req, response & res, const NextCallback next) {
  // respond with 500 "Internal Server Error".
  res.status(HttpStatus::SERVER_ERROR);
  res.send(err.message);
}

bool test = (app.get(F("env")) == "test");

void setup() {
  LOG_SETUP();

  ethernet_setup();
  

  app.get(F("/"), [](request & req, response & res, const NextCallback next) {
    // Caught and passed down to the errorHandler middleware
    throw new Error("something broke!");
  });

  app.get(F("/next"), [](request & req, response & res, const NextCallback next) {
    // We can also pass exceptions to next()
    // The reason for process.nextTick() is to show that
    // next() can be called inside an async operation,
    // in real life it can be a DB read or HTTP request.
    next(new Error("something else broke!"));
  });

  // Attach the first Error handling Middleware
  // function defined above (which logs the error)
  app.use(errorLogger);
  // Attach the second Error handling Middleware
  // function defined above (which sends back the response)
  app.use(errorResponder);

  app.listen(80, []() {
    LOG_I(F("Example app listening on port"), app.port);
  });
}

void loop() {
  app.run();
}