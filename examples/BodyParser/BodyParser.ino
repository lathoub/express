#define EX_DEBUG
#define EX_DEBUG_LOGLEVEL EX_DEBUG_LOGLEVEL_VERBOSE

#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

Express app;

void setup()
{
  Serial.begin(115200);
  while (!Serial && !Serial.available())  {  }

  Ethernet.init(5);
  Ethernet.begin(mac);

  app.use(bodyParser::json());

  app.post("/", [](Request & req, Response & res)
  {
    Serial.println(req.body);
    Serial.print(F("req.body: "));
    Serial.println(req.body);

    res.send(req.body);
  });

  app.listen(80, []()
  {
    Serial.print(F("Example app listening on port "));
    Serial.print(Ethernet.localIP());
    Serial.print(F(" "));
    Serial.println(app.port);
  });
}

void loop()
{
  app.run();
}