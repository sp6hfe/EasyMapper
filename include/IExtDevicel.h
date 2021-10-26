#pragma once

class IExtDevice {
public:
  virtual void enable() = 0;
  virtual void disable() = 0;
  virtual bool isEnabled() = 0;
};