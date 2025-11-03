#include "storage_config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Config config;

int loadConfig() {
  if (!LittleFS.begin()) return 0;
  if (!LittleFS.exists("/config.json")) return 2;

  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    Serial.println("❌ Falha ao abrir /config.json");
    return 0;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println("❌ Erro ao ler JSON!");
    return 0;
  }

  config.Id = doc["Id"].as<String>();
  config.IdUser = doc["IdUser"].as<String>();
  config.WifiSSID = doc["WifiSSID"].as<String>();
  config.WifiPassword = doc["WifiPassword"].as<String>();
  config.MqttHost = doc["MqttHost"].as<String>();
  config.MqttPort = doc["MqttPort"];
  config.MqttUser = doc["MqttUser"].as<String>();
  config.MqttPassword = doc["MqttPassword"].as<String>();
  config.BaseUrl = doc["BaseUrl"].as<String>();
  
  Serial.println("✅ Configuração carregada com sucesso!");
  return 1;
}

bool saveConfig() {
  File file = LittleFS.open("/config.json", "w");

  if (!file) {
    Serial.println("❌ Falha ao abrir arquivo para escrita!");
    return false;
  }

  JsonDocument doc;
  doc["Id"] = config.Id;
  doc["IdUser"] = config.IdUser;
  doc["WifiSSID"] = config.WifiSSID;
  doc["WifiPassword"] = config.WifiPassword;
  doc["MqttHost"] = config.MqttHost;
  doc["MqttPort"] = config.MqttPort;
  doc["MqttUser"] = config.MqttUser;
  doc["MqttPassword"] = config.MqttPassword;
  doc["BaseUrl"] = config.BaseUrl;

  if (serializeJson(doc, file) == 0) {
    Serial.println("❌ Falha ao salvar JSON!");
    file.close();
    return false;
  }

  file.close();
  Serial.println("💾 Configuração salva!");
  return true;
}

void deleteConfig() {
  if (!LittleFS.exists("/config.json")) return;
  
  LittleFS.remove("/config.json");
}

void printConfig() {
  Serial.println("📄 Configuração atual:");
  Serial.println("---------------------");
  Serial.println("Device ID: " + config.Id);
  Serial.println("User ID: " + config.IdUser);
  Serial.println("WiFi SSID: " + config.WifiSSID);
  Serial.println("MQTT Server: " + config.MqttHost);
  Serial.println("MQTT Port: " + String(config.MqttPort));
  Serial.println("MQTT User: " + String(config.MqttUser));
  Serial.println("MQTT Password: " + String(config.MqttPassword));
  Serial.println("BaseUrl: " + String(config.BaseUrl));
  Serial.println("---------------------");
}