#include "status_utils.h"
#include <Arduino.h>

static Status status = Status::INITIALIZING;

// FUNÇÕES EXTERNAS
extern void awaitingMobileConnection();
extern void connectServer();

void setStatus(Status newStatus) {
  if (newStatus == status) return;

  Serial.printf("🔁 Status alterado: %d → %d\n", (int)status, (int)newStatus);
  status = newStatus;

  if (newStatus == Status::AWAITING_CONNECTION) {
    awaitingMobileConnection();
  }

  if (newStatus == Status::ENABLED) {
    connectServer();
  }
}

Status getStatus() {
  return status;
}