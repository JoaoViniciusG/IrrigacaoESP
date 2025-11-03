#ifndef RTC_H
#define RTC_H

#include "types.h"
#include "RTClib.h"
#include "storage_config.h"

#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

extern RTC_DS3231 rtc;

void setupRTC(int sdaPort, int sclPort);
void syncRTCFromServer();

#endif