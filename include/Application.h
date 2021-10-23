#pragma once

#include "ILed.h"
#include "ILoRaWan.h"
#include "Stream.h"
#include <TinyGPS++.h>

namespace app {

class App {
public:
  static constexpr uint8_t PAYLOAD_SIZE = 11;

  struct dataToSend_t {
    double gpsLatitude;
    double gpsLongtitude;
    double gpsAltitude;
    double gpsHdop;
  };

private:
  ILed &led;
  ILoRaWan *lora;
  Stream *console;
  Stream *gpsLink;
  TinyGPSPlus gps;
  uint8_t payload[PAYLOAD_SIZE];
  dataToSend_t dataToSend;

  static void preparePayload(const dataToSend_t &data,
                             uint8_t (&payload)[PAYLOAD_SIZE]);

public:
  void setup();
  void loop(uint32_t loopEnterMillis);

  App(Stream *console_, Stream *gpsLink_, ILoRaWan *lora_, ILed &led_)
      : console(console_), gpsLink(gpsLink_), lora(lora_), led(led_){};
};

} // namespace app