#ifndef MOBILE_CONNECTION_H
#define MOBILE_CONNECTION_H

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#include "types.h"
#include "status_utils.h"
#include "utils.h"
#include "storage_config.h"

void awaitingMobileConnection();
void loopAwaitingConnection();

#endif