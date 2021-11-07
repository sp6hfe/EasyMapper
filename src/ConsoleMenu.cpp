#include "ConsoleMenu.h"

namespace app {

void ConsoleMenu::printInfo(Print &dataOut) {
  dataOut.println();
  dataOut.print("*****************************\n");
  dataOut.print("* LoRa EasyMapper by SP6HFE *\n");
  dataOut.print("*****************************\n");
  dataOut.println();
  dataOut.print("> type 'cfg' to enter config \n");
  dataOut.println();
}

bool ConsoleMenu::entryTriggerDetected(uint8_t dataIn) {
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

void ConsoleMenu::load(const ConsoleMenuEntry *menuEntries,
                       const uint8_t amountOfMenuEntries) {
  this->menuEntries = menuEntries;
  this->amountOfMenuEntries = amountOfMenuEntries;
}

void ConsoleMenu::display(Print &dataOut) const {
  if (this->menuEntries) {
    const uint8_t MENU_LETTERS_WIDE = 38;
    dataOut.println("+-----------------------------------+");
    dataOut.println("| Select an option or ESC to return |");
    dataOut.println("+-----------------------------------+");
    for (uint8_t index; index < this->amountOfMenuEntries; index++) {
      dataOut.print("| ");
      uint8_t lettersPrinted =
          dataOut.print(this->menuEntries[index].getDescription());
      for (; lettersPrinted < (MENU_LETTERS_WIDE - 4); lettersPrinted++) {
        dataOut.print(" ");
      }
      dataOut.println("|");
    }
    dataOut.println("+-----------------------------------+");
  }
}

bool ConsoleMenu::peform(uint8_t dataIn, Print &dataOut, config_t &config) {
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

  if (this->isActive) {
    if (this->currentMenuDisplayed) {
      // analyze inputs
    } else {
      this->display(dataOut);
      this->currentMenuDisplayed = true;
    }
  }

  return this->isActive;
}

} // namespace app
