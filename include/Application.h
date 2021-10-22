#pragma once

#include "ILed.h"
#include "ILoRaWan.h"
#include "Stream.h"

namespace app {

class App {
private:
  ILed &led;
  ILoRaWan *lora;
  Stream *console;
  Stream *gpsLink;

public:
  void setup();
  void loop();

  // TODO: remove after integrated into loop()
  void sendLoRaWanData(size_t dataSize, uint8_t *data, uint8_t port,
                       bool ifConfirmed);

  App(Stream *console_, Stream *gpsLink_, ILoRaWan *lora_, ILed &led_)
      : console(console_), gpsLink(gpsLink_), lora(lora_), led(led_){};
};

} // namespace app