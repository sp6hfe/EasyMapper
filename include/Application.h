#pragma once

#include "ILed.h"

namespace app {

class App {
private:
  ILed &led;

public:
  void init();
  void run();

  App(ILed &led_) : led(led_){};
};

} // namespace app