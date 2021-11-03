#pragma once

#include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

class SerialMenu {
private:
  const uint8_t ENTRY_TRIGGER_TEXT[4] = {'m', 'e', 'n', 'u'};
  bool isActive = false;

  bool entryTriggerDetected(uint8_t dataIn);

public:
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  SerialMenu(){};
};

} // namespace app
