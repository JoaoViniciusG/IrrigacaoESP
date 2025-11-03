#include "led.h"

unsigned long lastBlink = 0;
bool ledState = false;

int LED_GREEN;
int LED_RED;


void setupLedPin(int green, int red) {
  LED_GREEN = green;
  LED_RED = red;

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

void updateLeds(Status status) {
  unsigned long currentMillis = millis();
  unsigned long blinkInterval = 0;

  bool verde = false;
  bool vermelho = false;

  switch (status) {
    case Status::INITIALIZING:
      blinkInterval = 250;
      if (currentMillis - lastBlink >= blinkInterval) {
        ledState = !ledState;
        lastBlink = currentMillis;
      }
      verde = ledState;
      vermelho = !ledState;
      break;

    case Status::STANDBY:
      blinkInterval = 1000;
      if (currentMillis - lastBlink >= blinkInterval) {
        ledState = !ledState;
        lastBlink = currentMillis;
      }
      verde = ledState;
      break;

    case Status::AWAITING_CONNECTION:
      blinkInterval = 250;
      if (currentMillis - lastBlink >= blinkInterval) {
        ledState = !ledState;
        lastBlink = currentMillis;
      }
      verde = ledState;
      break;

    case Status::ENABLED:
      verde = true;
      break;

    case Status::DISABLED:
      blinkInterval = 1000;
      if (currentMillis - lastBlink >= blinkInterval) {
        ledState = !ledState;
        lastBlink = currentMillis;
      }
      vermelho = ledState;
      break;

    case Status::DISCONNECTED:
      vermelho = true;
      break;

    case Status::ERROR:
      blinkInterval = 250;
      if (currentMillis - lastBlink >= blinkInterval) {
        ledState = !ledState;
        lastBlink = currentMillis;
      }
      vermelho = ledState;
      break;
  }

  digitalWrite(LED_GREEN, verde ? HIGH : LOW);
  digitalWrite(LED_RED, vermelho ? HIGH : LOW);
}