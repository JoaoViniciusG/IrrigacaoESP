#ifndef LED_H
#define LED_H

#include "types.h"
#include <Arduino.h>

void setupLedPin(int green, int red);
void updateLeds(Status status);

#endif
