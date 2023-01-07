#include <Express.h>
using namespace EXPRESS_NAMESPACE;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Express express;

void setup() {
  Serial.begin(115200);
  while (!Serial) { }  delay(1500);

  Ethernet.begin(mac);

  express.get("/", [](Request &req, Response &res) {
    res.send("Hello World!"));
  });

  express.listen(3000, []() {
<<<<<<< Updated upstream
    Serial.print("Example app listening on port ");
    Serial.println(express.port);
=======
    Serial.println("Webserver on IP: listening on port:", express.port);
>>>>>>> Stashed changes
  });
}

void loop() {
  express.run();
}
