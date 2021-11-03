#pragma once

#include "Config.h"
#include "IGps.h"
#include "ILed.h"
#include "ILoRaWan.h"
#include "SerialMenu.h"
#include "Stream.h"

namespace app {

class App {
public:
  static constexpr uint8_t PAYLOAD_SIZE = 11;

private:
  config_t config;
  Stream &console;
  ILoRaWan &lora;
  IGps &gps;
  ILed &led;
  uint8_t payload[PAYLOAD_SIZE];
  IGps::gpsData_t gpsData;
  SerialMenu serialMenu;

  static void preparePayload(const IGps::gpsData_t &data,
                             uint8_t (&payload)[PAYLOAD_SIZE]);

public:
  void setup();
  void loop(uint32_t loopEnterMillis);

  App(Stream &console_, ILoRaWan &lora_, IGps &gps_, ILed &led_)
      : console(console_), lora(lora_), gps(gps_), led(led_){};
};

} // namespace app