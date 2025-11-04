#ifndef STATUS_UTILS_H
#define STATUS_UTILS_H

#include "types.h"
#include <Arduino.h>
#include "wifi_utils.h"
#include "mqtt_utils.h"

void setStatus(Status newStatus);
Status getStatus();

#endif