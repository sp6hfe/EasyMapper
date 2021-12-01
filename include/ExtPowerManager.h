#pragma once

#include "IGpsPower.h"
#include "ILedPower.h"
#include <Arduino.h>

namespace pwr {

class ExtPower : public IGpsPower, public ILedPower {
private:
  static constexpr uint8_t GPS_POWER_MASK = 0x01;
  static constexpr uint8_t LED_POWER_MASK = 0x02;
  static constexpr uint8_t ALL_POWER_MASK = 0xFF;
  static constexpr uint8_t EXT_POWER_PIN_NOT_SET = 0xFF;

  uint8_t extPowerPin = EXT_POWER_PIN_NOT_SET;
  uint8_t peripheralsPowered = 0;

  void setExtPower(uint8_t powerMask, bool enabled);

public:
  void begin(uint8_t extPowerPin_) {
    this->extPowerPin = extPowerPin_;
    if (this->extPowerPin != EXT_POWER_PIN_NOT_SET) {
      pinMode(this->extPowerPin, OUTPUT);
    }
    this->setExtPower(ALL_POWER_MASK, false);
  };

  // IGpsPower
  void gpsPowerOn() override;
  void gpsPowerOff() override;

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
  if (this->extPowerPin != EXT_POWER_PIN_NOT_SET) {
    if (this->peripheralsPowered) {
      digitalWrite(this->extPowerPin, LOW);
      delay(10);
    } else {
      digitalWrite(this->extPowerPin, HIGH);
    }
  }
}

void ExtPower::gpsPowerOn() { this->setExtPower(GPS_POWER_MASK, true); }

void ExtPower::gpsPowerOff() { this->setExtPower(GPS_POWER_MASK, false); }

void ExtPower::ledPowerOn() { this->setExtPower(LED_POWER_MASK, true); }

void ExtPower::ledPowerOff() { this->setExtPower(LED_POWER_MASK, false); }

} // namespace pwr