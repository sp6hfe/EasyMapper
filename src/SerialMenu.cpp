#include "SerialMenu.h"

namespace app {

bool SerialMenu::entryTriggerDetected(uint8_t dataIn) {
  static uint8_t entryTriggerIndex = 0;
  bool isDetected = false;

  if (dataIn == this->ENTRY_TRIGGER_TEXT[entryTriggerIndex++]) {
    if (entryTriggerIndex >= sizeof(this->ENTRY_TRIGGER_TEXT)) {
      entryTriggerIndex = 0;
      isDetected = true;
    }
  } else {
    entryTriggerIndex = 0;
    if (dataIn != this->ENTRY_TRIGGER_TEXT[entryTriggerIndex++]) {
      entryTriggerIndex = 0;
    }
  }

  return isDetected;
}

bool SerialMenu::peform(uint8_t dataIn, Print &dataOut, config_t &config) {
  if (!this->isActive) {
    this->isActive = this->entryTriggerDetected(dataIn);
  }

  return this->isActive;
}

} // namespace app
