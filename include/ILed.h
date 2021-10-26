#pragma once

#include "IExtDevicel.h"
#include <stdint.h>

class ILed : public IExtDevice {
public:
  static constexpr uint32_t LED_RED = 0x00FF0000;
  static constexpr uint32_t LED_GREEN = 0x0000FF00;
  static constexpr uint32_t LED_BLUE = 0x000000FF;

  virtual void on() = 0;
  virtual void off() = 0;
  virtual void setColor(const uint32_t color) = 0;
  virtual void setBrightness(const uint8_t level) = 0;
};