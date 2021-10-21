#include "Application.h"

namespace app {

void App::setup() {
  this->led.setColor(ILed::LED_BLUE);
  this->console->print("\nLoRa EasyMapper by SP6HFE\n");
}

void App::loop() {}

} // namespace app