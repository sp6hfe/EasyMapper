#pragma once

#include "ILed.h"
#include "ILedPower.h"
#include <Arduino.h>
#include <CubeCell_NeoPixel.h>

namespace wrappers {
class Led : public ILed {
private:
  static constexpr uint32_t LED_OFF = 0x00000000;

  CubeCell_NeoPixel led = CubeCell_NeoPixel(1, RGB, NEO_GRB + NEO_KHZ800);
  uint32_t lastColor = LED_OFF;
  bool enabled = false;
  ILedPower &ledPower;

public:
  void on() override;
  void off() override;
  void setColor(const uint32_t color) override;
  void setBrightness(const uint8_t level) override;
  void enable() override;
  void disable() override;
  bool isEnabled() override;

  Led(ILedPower &ledPower_) : ledPower(ledPower_) { this->disable(); }
};

void Led::on() {
  if (this->enabled) {
    this->setColor(this->lastColor);
  }
}

void Led::off() {
  if (this->enabled) {
    this->setColor(LED_OFF);
  }
}

void Led::setColor(const uint32_t color) {
  if (this->enabled) {
    uint8_t red = (uint8_t)(color >> 16);
    uint8_t green = (uint8_t)(color >> 8);
    uint8_t blue = (uint8_t)color;
    this->led.setPixelColor(0, this->led.Color(red, green, blue));
    this->led.show();
    this->lastColor = color;
  }
}

void Led::setBrightness(const uint8_t level) { this->led.setBrightness(level); }

void Led::enable() {
  this->ledPower.ledPowerOn();
  this->led.begin();
  this->led.clear();
  this->led.setBrightness(10);
  this->setColor(LED_OFF);
  this->enabled = true;
}

void Led::disable() {
  this->ledPower.ledPowerOff();
  this->enabled = false;
}

bool Led::isEnabled() { return this->enabled; }

} // namespace wrappers
