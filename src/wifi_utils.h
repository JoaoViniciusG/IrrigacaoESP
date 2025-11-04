#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "rtc.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "status_utils.h"

void setupWiFi(const char* ssid, const char* password);
bool checkWifi();
void loopWiFi();

#endif