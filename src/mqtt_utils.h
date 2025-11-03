#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "status_utils.h"
#include <ArduinoJson.h>
#include "status_utils.h"
#include "rele.h"
#include "soil.h"
#include "storage_config.h"

extern bool toConnectMqtt;

void mqttInit(const char* server, int port, const char* userid, const char* username, const char* password, const char* name);
void mqttLoop();
void mqttPublish(String topic, String message);

#endif
