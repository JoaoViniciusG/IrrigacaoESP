#include "wifi_utils.h"

void setupWiFi(String ssid, String password) {
  Serial.printf("Conectando a %s", ssid.c_str());
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.printf("\nConectado! IP: %s\n", WiFi.localIP().toString().c_str());

  delay(1000);
  syncRTCFromServer();
}