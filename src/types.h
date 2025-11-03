#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

// =================== ENUMS GLOBAIS ===================
enum class Status {
  INITIALIZING = 0,
  STANDBY = 1,
  AWAITING_CONNECTION = 2,
  ENABLED = 3,
  DISABLED = 4,
  DISCONNECTED = 5,
  ERROR = 6
};

// =================== STRUCTS GLOBAIS ===================
struct Config {
  String Id;
  String IdUser;
  String WifiSSID;
  String WifiPassword;
  String MqttHost;
  int MqttPort;
  String MqttUser;
  String MqttPassword;
  String BaseUrl;
};

// =================== TYPEDEFS ===================
typedef unsigned long timestamp_t;
typedef void (*Callback)();

#endif