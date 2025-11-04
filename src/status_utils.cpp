#include "status_utils.h"

static Status status = Status::INITIALIZING;

// FUNÇÕES EXTERNAS
extern void awaitingMobileConnection();
extern void connectWifi();
extern bool ESP_ENABLED;
extern bool toConnectMqtt;

void setStatus(Status newStatus)
{
  if (newStatus == status)
    return;

  Serial.printf("🔁 Status alterado: %d → %d\n", (int)status, (int)newStatus);
  status = newStatus;

  if (newStatus == Status::AWAITING_CONNECTION)
  {
    awaitingMobileConnection();
  }

  if (newStatus == Status::ENABLED || newStatus == Status::DISABLED)
  {
    ESP_ENABLED = newStatus == Status::ENABLED;
    if (!checkWifi())
    {
      setStatus(Status::DISCONNECTED);
      return;
    }

    toConnectMqtt = true;
  }
  else
  {
    toConnectMqtt = false;
  }

  if (newStatus == Status::DISCONNECTED)
  {
    connectWifi();
  }
}

Status getStatus()
{
  return status;
}