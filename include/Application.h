#pragma once

#include "ILed.h"
#include "Stream.h"

namespace app {

class App {
private:
  ILed &led;
  Stream *console;
  Stream *gpsLink;

public:
  void setup();
  void loop();

  App(Stream *console_, Stream *gpsLink_, ILed &led_)
      : console(console_), gpsLink(gpsLink_), led(led_){};
};

} // namespace app