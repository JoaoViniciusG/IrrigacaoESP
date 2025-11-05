#include "scheduler.h"

#define FILE_PATH "/agendamentos.json"

extern RTC_DS3231 rtc;
extern bool ESP_ENABLED;

std::vector<Schedule> schedules;
DateTime lastTimeChecked;

// ------------------------
// 🔹 Carrega JSON do LittleFS
// ------------------------
bool loadSchedules()
{
  if (!LittleFS.begin())
  {
    Serial.println("❌ Falha ao iniciar LittleFS!");
    return false;
  }

  if (!LittleFS.exists(FILE_PATH))
  {
    Serial.println("⚠️ Nenhum arquivo de agendamento encontrado.");
    return false;
  }

  File file = LittleFS.open(FILE_PATH, "r");
  if (!file)
  {
    Serial.println("❌ Falha ao abrir agendamentos.json!");
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err)
  {
    Serial.println("❌ Erro ao ler agendamentos.json!");
    Serial.println(err.c_str());
    return false;
  }

  schedules.clear();
  for (JsonVariant a : doc.as<JsonArray>())
  {
    Schedule s;
    s.id = a["id"].as<String>();
    s.time = a["time"].as<String>();
    s.days = a["days"];
    s.duration = a["duration"];
    s.status = a["status"];
    s.executedToday = false;
    schedules.push_back(s);
  }

  Serial.printf("✅ %d agendamentos carregados!\n", schedules.size());
  return true;
}

// ------------------------
// 💾 Salva JSON no LittleFS
// ------------------------
bool saveSchedules()
{
  File file = LittleFS.open(FILE_PATH, "w");
  if (!file)
  {
    Serial.println("❌ Falha ao abrir arquivo para escrita!");
    return false;
  }

  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (auto &s : schedules)
  {
    JsonObject o = arr.add<JsonObject>();
    o["id"] = s.id;
    o["time"] = s.time;
    o["days"] = s.days;
    o["duration"] = s.duration;
    o["status"] = s.status;
  }

  if (serializeJsonPretty(doc, file) == 0)
  {
    Serial.println("❌ Falha ao salvar agendamentos!");
    file.close();
    return false;
  }

  file.close();
  Serial.println("💾 Agendamentos salvos!");
  return true;
}

// ------------------------
// 🔁 Função a ser chamada no loop principal
// ------------------------
void loopScheduler()
{
  if (!ESP_ENABLED)
    return;

  static unsigned long lastCheck = 0;
  unsigned long nowMillis = millis();

  // 🔹 Executa a verificação apenas a cada 5 segundos
  if (nowMillis - lastCheck < 5000)
    return;
  lastCheck = nowMillis;

  DateTime now = rtc.now();

  // 🔹 Reseta flags à meia-noite
  if (lastTimeChecked.day() != now.day())
  {
    resetDailyFlags();
  }
  lastTimeChecked = now;

  // 🔹 Bitmask do dia atual (Dom=1, Seg=2, Ter=4, ...)
  uint8_t todayBit = 1 << ((now.dayOfTheWeek() + 6) % 7);

  // 🔹 Hora atual sem Strings (eficiente)
  uint8_t hour = now.hour();
  uint8_t minute = now.minute();

  for (auto &s : schedules)
  {
    if (!s.status || s.executedToday)
      continue;
    if (!(s.days & todayBit))
      continue;

    // Converte "HH:MM" -> valores numéricos
    uint8_t schedHour = (s.time.substring(0, 2)).toInt();
    uint8_t schedMinute = (s.time.substring(3, 5)).toInt();

    if (hour == schedHour && minute == schedMinute)
    {
      Serial.printf("⏰ Executando agendamento %s\n", s.id.c_str());
      executeSchedule(s);
      s.executedToday = true;
    }
  }
}

// ------------------------
// 🕒 Executa a ação (substitua com seu controle real)
// ------------------------
void executeSchedule(const Schedule &s)
{
  static bool isActive = false;
  static unsigned long startTime = 0;

  if (!isActive)
  {
    Serial.printf("🚀 Iniciando agendamento %s (%d s)\n", s.id.c_str(), s.duration);
    digitalWrite(D1, HIGH);
    startTime = millis();
    isActive = true;
  }

  // desliga após o tempo definido
  if (isActive && millis() - startTime >= s.duration * 1000UL)
  {
    digitalWrite(D1, LOW);
    isActive = false;
    Serial.println("✅ Agendamento concluído.");
  }
}

// ------------------------
// 🔄 Reseta flags diárias
// ------------------------
void resetDailyFlags()
{
  for (auto &s : schedules)
    s.executedToday = false;
  Serial.println("📆 Flags diárias resetadas.");
}

void syncRTCFromServer()
{
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String URL = config.BaseUrl + "/agendamento/controlador";
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



}