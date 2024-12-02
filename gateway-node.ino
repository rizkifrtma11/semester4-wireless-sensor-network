/*
# penerima gateway
*/

#include <ESP8266WiFi.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("======[ Terhubung ]======");
    while (client.connected()) {
      if (client.available()) {
        String data = client.readStringUntil('\n');
        Serial.println(data);
      }
    }
    client.stop();
    Serial.println("------[ Terputus ]------");
  }
}