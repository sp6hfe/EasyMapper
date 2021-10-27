#pragma once

class IGpsPower {
public:
  virtual void gpsPowerOn() = 0;
  virtual void gpsPowerOff() = 0;
};