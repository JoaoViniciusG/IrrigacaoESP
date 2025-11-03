#include "soil.h"

const unsigned long INTERVAL_MS = 10000UL;
const unsigned long STABILIZE_DELAY = 500;

unsigned long lastReadMillis = 0;
unsigned long sensorOnMillis = 0;
bool sensorOn = false;
bool waitingToRead = false;

int SOIL_VCC_PORT;
int SOIL_DAT_PORT;

int lastMoistureReaded;

void setupSoil(int portVcc, int portAnalog)
{
  SOIL_DAT_PORT = portAnalog;
  SOIL_VCC_PORT = portVcc;

  pinMode(SOIL_VCC_PORT, OUTPUT);
  digitalWrite(SOIL_VCC_PORT, LOW);
}

int readMoisturePercent()
{
  digitalWrite(SOIL_VCC_PORT, HIGH);
  delay(500);

  int analogValue = analogRead(SOIL_DAT_PORT);
  digitalWrite(SOIL_VCC_PORT, LOW);

  Serial.println(analogValue);

  int moisturePercent = map(analogValue, 1024, 520, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  return moisturePercent;
}

void loopSoil()
{
  unsigned long now = millis();

  if (!sensorOn && !waitingToRead && now - lastReadMillis >= INTERVAL_MS) {
    digitalWrite(SOIL_VCC_PORT, HIGH);
    sensorOnMillis = now;
    sensorOn = true;
    waitingToRead = true;
  }

  if (waitingToRead && now - sensorOnMillis >= STABILIZE_DELAY) {
    int analogValue = analogRead(SOIL_DAT_PORT);
    digitalWrite(SOIL_VCC_PORT, LOW);
    sensorOn = false;
    waitingToRead = false;
    lastReadMillis = now;

    int moisturePercent = map(analogValue, 1024, 520, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);
  
    lastMoistureReaded = moisturePercent;
  }

  yield();
}