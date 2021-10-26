#pragma once

#include "ILedPower.h"
#include <Arduino.h>

namespace pwr {

class ExtPower : public ILedPower {
private:
  static constexpr uint8_t LED_POWER_MASK = 0x01;
  static constexpr uint8_t GPS_POWER_MASK = 0x02;
  static constexpr uint8_t EXT_POWER_PIN_NOT_SET = 0xFF;

  uint8_t extPowerPin = EXT_POWER_PIN_NOT_SET;
  uint8_t peripheralsPowered = 0;

  void setExtPower(uint8_t powerMask, bool enabled);

public:
  void begin(uint8_t extPowerPin_) { extPowerPin = extPowerPin_; };

  // ILedPower
  void ledPowerOn() override;
  void ledPowerOff() override;

  ExtPower(){};
};

void ExtPower::setExtPower(uint8_t powerMask, bool enabled) {
  if (enabled) {
    this->peripheralsPowered |= powerMask;
  } else {
    this->peripheralsPowered &= ~powerMask;
  }

  // LED and GPS share the same external power rail
  if (peripheralsPowered) {
    digitalWrite(extPowerPin, 0);
    delay(1);
  } else {
    digitalWrite(extPowerPin, 1);
  }
}

void ExtPower::ledPowerOn() { this->setExtPower(LED_POWER_MASK, true); }

void ExtPower::ledPowerOff() { this->setExtPower(LED_POWER_MASK, false); }

} // namespace pwr