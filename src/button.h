#ifndef BUTTON_H
#define BUTTON_H

#include "rele.h"
#include "types.h"
#include <Arduino.h>
#include "status_utils.h"

void setupButtonPin(int port);
void handleButton(Status status);

#endif