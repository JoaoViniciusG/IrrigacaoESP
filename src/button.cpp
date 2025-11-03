#include "button.h"

unsigned long buttonPressStart = 0;
bool buttonPressed = false;
bool threeSecondsTriggered = false;
bool tenSecondsTriggered = false;
int BUTTON_PIN;

extern void changeReleState();

void setupButtonPin(int port) {
  BUTTON_PIN = port;
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void handleButton(Status status) {
  bool reading = digitalRead(BUTTON_PIN);

  // AO PRESSIONAR O BOTÃO //
  if (reading == LOW && !buttonPressed) {
    buttonPressed = true;
    buttonPressStart = millis();
    threeSecondsTriggered = false;
    tenSecondsTriggered = false;
  }

  // ENQUANTO O BOTÃO ESTÁ PRESSIONADO //
  if (buttonPressed && reading == LOW) {
    unsigned long holdTime = millis() - buttonPressStart;

    // ATIVAÇÃO DE 3S //
    if (holdTime >= 3000 && !threeSecondsTriggered) {
      threeSecondsTriggered = true;

      if (status == Status::STANDBY) {
        setStatus(Status::AWAITING_CONNECTION);
        Serial.println("🔄 STANDBY → AWAITING_CONNECTION (3s)");
      }
    }

    // ATIVAÇÃO DE 10S //
    if (holdTime >= 10000 && !tenSecondsTriggered) {
      tenSecondsTriggered = true;

      if (status != Status::STANDBY && status != Status::AWAITING_CONNECTION) {
        setStatus(Status::AWAITING_CONNECTION);
        Serial.println("🚨 Qualquer estado → AWAITING_CONNECTION (10s)");
      }
    }
  }

  // AO SOLTAR O BOTÃO //
  if (reading == HIGH && buttonPressed) {
    unsigned long pressDuration = millis() - buttonPressStart;

    // ATIVAÇÃO DE CURTA DURAÇÃO //
    if (pressDuration < 3000) 
    {
      changeReleState();  
    }

    buttonPressed = false;
  }
}