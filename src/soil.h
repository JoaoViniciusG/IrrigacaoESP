#ifndef SOIL_H
#define SOIL_H

#include "types.h"
#include <Arduino.h>

extern int lastMoistureReaded;

void setupSoil(int portVcc, int portAnalog);
int readMoisturePercent();
void loopSoil();

#endif