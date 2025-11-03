#ifndef RELE_H
#define RELE_H

#include "types.h"
#include <Arduino.h>

extern bool RELE_STATE;

void setupRele(int port);
void changeReleState();
void disabledIfDanger();
void loopRele();
void controlRele(bool state, int tempoSegundos);

#endif