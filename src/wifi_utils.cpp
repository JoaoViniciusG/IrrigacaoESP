#include "wifi_utils.h"

bool wifiConnected = false;
unsigned long lastWifiCheck = 0;

void setupWiFi(const char* ssid, const char* password) {
  Serial.printf("Conectando a %s", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiConnected = false;
}

void loopWiFi() {
  if (!wifiConnected && getStatus() == Status::DISCONNECTED) {
    unsigned long now = millis();
    
    if (now - lastWifiCheck > 500) {
      lastWifiCheck = now;
      wl_status_t status = WiFi.status();

      if (status == WL_CONNECTED) {
        wifiConnected = true;
        setStatus(Status::ENABLED);

        Serial.printf("\n✅ Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
        syncRTCFromServer();
      }
    }
  }
}

bool checkWifi()
{
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    return client.connect("www.google.com", 80);
  }
  else return false;
}