/*
# END NODE
*/

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define DHTPIN 4       // Pin yang terhubung ke pin data DHT11
#define DHTTYPE DHT11  // Jenis sensor DHT11
#define LEDPIN 5       // Pin yang terhubung ke LED (D1 = GPIO5)

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* host = "IP_ADDR_GATEWAY_NODE"; // Ganti dengan IP address server ESP8266 (Central Node)

WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 7 * 3600; // Offset untuk WIB (UTC+7)
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000); // Update setiap 60 detik

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  dht.begin();

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW); // Mulai dengan LED mati

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
}

void loop() {
  timeClient.update();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca data DHT!");
    return;
  }

  // Kontrol LED berdasarkan suhu
  String fanStatus;
  if (t > 33.0) {
    digitalWrite(LEDPIN, HIGH); // Menyalakan LED
    fanStatus = "Fan status: ON";
  } else {
    digitalWrite(LEDPIN, LOW); // Mematikan LED
    fanStatus = "Fan status: OFF";
  }

  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("Koneksi Gagal.");
    delay(1000);
    return;
  }

  Serial.print("Mengirim data ke Node ");
  Serial.println(host);

  // Dapatkan waktu dari NTP
  String formattedTime = timeClient.getFormattedTime();
  int separatorIndex = formattedTime.indexOf(":");
  String hours = formattedTime.substring(0, separatorIndex);
  String minutes = formattedTime.substring(separatorIndex + 1, formattedTime.lastIndexOf(":"));

  // Format data yang akan dikirim
  String data = "Time: " + hours + ":" + minutes + "\n" + fanStatus + "\n" + "Temp: " + String(t) + "*C | Humid: " + String(h) + "%";

  // Mengirim data ke server
  client.print(data + "\n");

  client.stop();
  Serial.println("Data terkirim. Menutup koneksi.");
  delay(5000); // Tunggu 5 detik sebelum mengirim lagi
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
}