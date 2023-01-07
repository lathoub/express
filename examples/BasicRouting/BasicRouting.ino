#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express express;

void setup()
{
  Serial.begin(115200);
  while (!Serial)  { } delay(1500);

  Ethernet.begin(mac);

  express.get("/", [](Request &req, Response &res)
              { res.send("Hello World!"); });

  express.post("/", [](Request &req, Response &res)
              { res.send("Got a POST request"); });

  express.put("/user", [](Request &req, Response &res)
              { res.send("Got a PUT request at /user"); });

  express.delete("/user", [](Request &req, Response &res)
              { res.send("Got a DELETE request at /user"); });

  express.listen(80, []()
                 { Serial.println("Webserver on IP:", Ethernet.localIP(), "listening on port:", express.port); });
}

void loop()
{
  express.run();
}
