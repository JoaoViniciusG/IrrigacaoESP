#ifndef STORAGE_CONFIG_H
#define STORAGE_CONFIG_H

#include "types.h"
#include <Arduino.h>

extern Config config;

int loadConfig();
bool saveConfig();
void deleteConfig();
void printConfig();

#endif