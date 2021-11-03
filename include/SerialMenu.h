#pragma once

#include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

class SerialMenu {
private:
  static constexpr uint8_t ESC_KEY_CODE = 0x1B;
  const uint8_t ENTRY_TRIGGER_TEXT[3] = {'c', 'f', 'g'};
  bool isActive = false;

  void printInfo(Print &dataOut);
  bool entryTriggerDetected(uint8_t dataIn);

public:
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  SerialMenu(){};
};

} // namespace app
