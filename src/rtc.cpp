#include "rtc.h"

RTC_DS3231 rtc;

void setupRTC(int sdaPort, int sclPort)
{
  Wire.begin(sdaPort, sclPort);

  if (!rtc.begin()) {
    Serial.println("❌ Falha ao inicializar RTC DS3231!");
  }
}

void syncRTCFromServer()
{
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String URL = config.BaseUrl + "/time";
  Serial.printf("🌐 Conectando a %s\n", URL.c_str());
  if (!http.begin(client, URL))
  {
    Serial.println("❌ Falha ao iniciar conexão HTTP para o RTC");
    return;
  }

  int code = http.GET();
  if (code != HTTP_CODE_OK)
  {
    Serial.printf("❌ Erro HTTP: %d\n", code);
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err)
  {
    Serial.println("❌ Falha ao interpretar JSON do WorldTimeAPI");
    return;
  }

  String datetime = doc["datetime"] | "";
  if (datetime == "")
  {
    Serial.println("⚠️ Campo 'datetime' não encontrado");
    return;
  }

  int year, month, day, hour, minute, second;
  if (sscanf(datetime.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d",
             &year, &month, &day, &hour, &minute, &second) == 6)
  {
    rtc.adjust(DateTime(year, month, day, hour, minute, second));
  }
  else
  {
    Serial.println("⚠️ Falha ao interpretar a data/hora recebida!");
    return;
  }

  // Ajusta o RTC
  rtc.adjust(DateTime(year, month, day, hour, minute, second));

  Serial.printf("✅ RTC atualizado: %02d/%02d/%04d %02d:%02d:%02d\n",
                day, month, year, hour, minute, second);
}