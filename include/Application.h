#pragma once

#include "ILed.h"
#include "Stream.h"

namespace app {

class App {
private:
  ILed &led;
  Stream *console;

public:
  void setup();
  void loop();

  App(Stream *console_, ILed &led_) : console(console_), led(led_){};
};

} // namespace app