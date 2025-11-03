#include "utils.h"
#include <ESP8266WiFi.h>

String getIdentifier() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String chipId = String(ESP.getChipId(), HEX);
  return mac + "-" + chipId;
}