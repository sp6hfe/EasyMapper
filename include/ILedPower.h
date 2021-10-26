#pragma once

class ILedPower {
public:
  virtual void ledPowerOn() = 0;
  virtual void ledPowerOff() = 0;
};