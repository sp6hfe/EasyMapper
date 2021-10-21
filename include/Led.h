#pragma once

#include "ILed.h"
#include <Arduino.h>
#include <CubeCell_NeoPixel.h>

namespace wrappers {
class Led : public ILed {
private:
  static constexpr uint32_t LED_OFF = 0x00000000;

  CubeCell_NeoPixel led = CubeCell_NeoPixel(1, RGB, NEO_GRB + NEO_KHZ800);
  uint32_t lastColor = 0;

public:
  void on() override;
  void off() override;
  void setColor(const uint32_t color) override;
  void setBrightness(const uint8_t level) override;

  Led() {
    this->led.begin();
    this->led.clear();
    this->led.setBrightness(10);
  }
};
} // namespace wrappers
