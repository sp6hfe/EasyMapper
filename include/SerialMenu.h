#pragma once

#include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

class SerialMenu {
public:
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  SerialMenu(){};
};

} // namespace app
