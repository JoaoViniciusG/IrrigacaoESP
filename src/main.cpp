#include <Arduino.h>
#include "mobile_connection.h"
#include "storage_config.h"
#include "wifi_utils.h"
#include "mqtt_utils.h"
#include "button.h"
#include "utils.h"
#include "types.h"
#include "rele.h"
#include "led.h"
#include "rtc.h"
#include "soil.h"

const int LED_RED = D1;
const int LED_GREEN = D2;
const int BUTTON = D3;
const int RELE = D4;
const int RTC_SDA = D5;
const int RTC_SCL = D6;
const int SOIL_ANALOG = A0;
const int SOIL_VCC = D7;

String identifier = getIdentifier();

void setup()
{
  // STARTING SERIAL //
  Serial.begin(115200);
  
  // LOADING CONFIG //
  int configStatus = loadConfig();
  config.IdUser = "8860e4fe-b6ca-11f0-b0a9-862ccfb01bc1";
  saveConfig();
  printConfig();

  // DEFINITIONS //
  setupLedPin(LED_GREEN, LED_RED);
  setupButtonPin(BUTTON);
  setupRele(RELE);
  setupRTC(RTC_SDA, RTC_SCL);
  setupSoil(SOIL_VCC, SOIL_ANALOG);

  // VERIFY CONFIG STATUS //
  if (configStatus != 1)
  {
    setStatus((configStatus == 0) ? Status::ERROR : Status::STANDBY);
    return;
  }

  setStatus(Status::ENABLED);
}

void loop()
{
  Status status = getStatus();

  loopRele();
  loopSoil();
  mqttLoop();
  handleButton(getStatus());
  updateLeds(getStatus());

  if (status == Status::AWAITING_CONNECTION)
  {
    loopAwaitingConnection();
  }
}

void connectServer()
{
  setupWiFi(config.WifiSSID, config.WifiPassword);

  mqttInit(config.MqttHost.c_str(), config.MqttPort, config.IdUser.c_str(), config.MqttUser.c_str(), config.MqttPassword.c_str(), String(identifier + "_" + config.Id).c_str());
}