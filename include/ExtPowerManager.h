#pragma once

#include "IGpsPower.h"
#include "ILedPower.h"
#include <Arduino.h>

namespace pwr {

class ExtPower : public IGpsPower, public ILedPower {
public:
  static constexpr uint8_t EXT_POWER_PIN_UNUSED = 0xFF;

private:
  static constexpr uint8_t EXT_POWER_PIN_LEVEL_ON = LOW;
  static constexpr uint8_t EXT_POWER_PIN_LEVEL_OFF = HIGH;
  static constexpr uint8_t EXT_POWER_ON_DELAY_MS = 10;

  static constexpr uint8_t GPS_POWER_MASK = 0x01;
  static constexpr uint8_t LED_POWER_MASK = 0x02;
  static constexpr uint8_t ALL_POWER_MASK = 0xFF;

  uint8_t extPowerPin;
  uint8_t peripheralsPowered = 0;

  void setExtPower(uint8_t powerMask, bool enabled);

public:
  // IGpsPower
  void gpsPowerOn() override;
  void gpsPowerOff() override;

  // ILedPower
  void ledPowerOn() override;
  void ledPowerOff() override;

  ExtPower(uint8_t extPowerPin_) : extPowerPin(extPowerPin_) {
    if (this->extPowerPin != EXT_POWER_PIN_UNUSED) {
      pinMode(this->extPowerPin, OUTPUT);
    }
    this->setExtPower(ALL_POWER_MASK, false);
  };
};

void ExtPower::setExtPower(uint8_t powerMask, bool enabled) {
  bool powerOnDelay = false;

  if (this->extPowerPin == EXT_POWER_PIN_UNUSED) {
    return;
  }

  // LED and GPS share the same external power rail

  if (enabled) {
    if (!this->peripheralsPowered && powerMask) {
      // on true power-on additional delay is added for ext devices to init
      powerOnDelay = true;
    }
    this->peripheralsPowered |= powerMask;
  } else {
    this->peripheralsPowered &= ~powerMask;
  }

  if (this->peripheralsPowered) {
    digitalWrite(this->extPowerPin, EXT_POWER_PIN_LEVEL_ON);
    if (powerOnDelay) {
      delay(EXT_POWER_ON_DELAY_MS);
    }
  } else {
    digitalWrite(this->extPowerPin, EXT_POWER_PIN_LEVEL_OFF);
  }
}

void ExtPower::gpsPowerOn() { this->setExtPower(GPS_POWER_MASK, true); }

void ExtPower::gpsPowerOff() { this->setExtPower(GPS_POWER_MASK, false); }

void ExtPower::ledPowerOn() { this->setExtPower(LED_POWER_MASK, true); }

void ExtPower::ledPowerOff() { this->setExtPower(LED_POWER_MASK, false); }

} // namespace pwr