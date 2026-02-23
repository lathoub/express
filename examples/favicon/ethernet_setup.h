#ifdef PLATFORM_ESP32
#include "arduino_secrets.h"
#endif

#ifdef PLATFORM_ESP32_W5500
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
#endif

#ifdef PLATFORM_ESP32_W5500
void ethernet_setup() {
  Ethernet.init(5);
  Ethernet.begin(mac);
  
  LOG_I(F("IP address"), Ethernet.localIP());
}
#endif

#ifdef PLATFORM_ESP32
void ethernet_setup() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  LOG_I(F("IP address"), WiFi.localIP());
}
#endif