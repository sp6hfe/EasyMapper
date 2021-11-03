#include "SerialMenu.h"

namespace app {

void SerialMenu::printInfo(Print &dataOut) {
  dataOut.println();
  dataOut.print("*****************************\n");
  dataOut.print("* LoRa EasyMapper by SP6HFE *\n");
  dataOut.print("*****************************\n");
  dataOut.println();
  dataOut.print("> type 'cfg' to enter config \n");
  dataOut.println();
}

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
    switch (dataIn) {
    case '\n':
      // ENTER code id 0x0D0A so here we have already processed 0x0D by
      // entryTriggerDetected
      this->printInfo(dataOut);
      break;
    default:
      this->isActive = this->entryTriggerDetected(dataIn);
      break;
    }
  }

  return this->isActive;
}

} // namespace app
