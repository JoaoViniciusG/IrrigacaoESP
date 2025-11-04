#include "rele.h"

extern bool ESP_ENABLED;

bool RELE_STATE = LOW;
bool RELE_DANGER = false;
bool RELE_ACTIVATED_TIMER = LOW;
int RELE_PIN;

unsigned long RELE_OFF_TIME = 0;

void setupRele(int port)
{
  RELE_PIN = port;
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, RELE_STATE);
}

void disabledIfDanger()
{
  if (RELE_STATE == HIGH && RELE_DANGER)
  {
    changeReleState();
  }
}

void setReleState()
{
  if (RELE_STATE == LOW)
  {
    RELE_DANGER = false;
  }
  digitalWrite(RELE_PIN, RELE_STATE);
}

void changeReleState()
{
if (!ESP_ENABLED) return;

  RELE_STATE = !RELE_STATE;
  setReleState();
}

void controlRele(bool state, int tempoSegundos)
{
  if (!ESP_ENABLED) return;

  if (state)
  {
    RELE_STATE = HIGH;
    setReleState();

    if (tempoSegundos > 0)
    {
      RELE_OFF_TIME = millis() + (unsigned long)tempoSegundos * 1000UL;
      RELE_ACTIVATED_TIMER = HIGH;
      RELE_DANGER = false;
    }
    else
    {
      RELE_DANGER = true;
      RELE_ACTIVATED_TIMER = LOW;
      RELE_OFF_TIME = 0;
    }
  }
  else
  {
    RELE_STATE = LOW;
    setReleState();
    RELE_DANGER = false;
    RELE_ACTIVATED_TIMER = LOW;
    RELE_OFF_TIME = 0;
  }
}

void loopRele()
{
  if (!ESP_ENABLED)
  {
    if (RELE_STATE == HIGH) changeReleState();
    return;
  }

  if (RELE_ACTIVATED_TIMER && millis() >= RELE_OFF_TIME)
  {
    RELE_STATE = LOW;
    setReleState();
    RELE_ACTIVATED_TIMER = LOW;
    RELE_OFF_TIME = 0;
  }
}
