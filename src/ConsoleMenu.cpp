#include "ConsoleMenu.h"

namespace app {

bool ConsoleMenu::entryTextDetection(uint8_t dataIn) {
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

void ConsoleMenu::display(Print &dataOut) const {
  if (this->isActive) {
    const uint8_t MENU_LETTERS_WIDE = 38;
    dataOut.println("+-----------------------------------+");
    dataOut.println("| Select an option or ESC to return |");
    dataOut.println("+-----------------------------------+");
    if (this->newMenuToDisplay) {
      if (this->menuEntries) {
        for (uint8_t index; index < this->amountOfMenuEntries; index++) {
          dataOut.print("| ");
          uint8_t lettersPrinted =
              dataOut.print(this->menuEntries[index].getDescription());
          for (; lettersPrinted < (MENU_LETTERS_WIDE - 4); lettersPrinted++) {
            dataOut.print(" ");
          }
          dataOut.println("|");
        }
      } else { // if (this->menuEntries) {
        dataOut.println("|  no options - menu misconfigured  |");
      }
    } else { // if (this->newMenuToDisplay) {
      dataOut.println("|  no options - menu wasn't loaded  |");
    }
    dataOut.println("+-----------------------------------+");
  } else { // if (this->isActive) {
    dataOut.println("+-----------------------------------+");
    dataOut.println("|     LoRa EasyMapper by SP6HFE     |");
    dataOut.println("+-----------------------------------+");
    dataOut.println("| type 'cfg' to enter configuration |");
    dataOut.println("+-----------------------------------+");
  }
}

bool ConsoleMenu::processInput(uint8_t dataIn) {
  bool ifMenuActive = true;

  // lookup for action to execute based on action key
  for (uint8_t index; index < this->amountOfMenuEntries; index++) {
    if (dataIn == this->menuEntries[index].getActionKey()) {
      switch (this->menuEntries[index].getEntryType()) {
      case ConsoleMenuEntryType::SUBMENU:
        this->menuEntries[index].makeAction(reinterpret_cast<void *>(this));
        break;
      case ConsoleMenuEntryType::EXIT:
        ifMenuActive = false;
        break;
      case ConsoleMenuEntryType::NONE:
        /* fallthrough */
      default:
        // no action for unknown types
        break;
      }
    }
  }

  return ifMenuActive;
}

void ConsoleMenu::load(const ConsoleMenuEntry *menuEntries,
                       const uint8_t amountOfMenuEntries) {
  this->menuEntries = menuEntries;
  this->amountOfMenuEntries = amountOfMenuEntries;
  this->newMenuToDisplay = true;
}

bool ConsoleMenu::peform(uint8_t dataIn, Print &dataOut, config_t &config) {
  if (!this->isActive) {
    switch (dataIn) {
    case '\n':
      // ENTER code id 0x0D0A so here we have already processed 0x0D by
      // entryTriggerDetected
      this->display(dataOut);
      break;
    default:
      // parse input looking for menu entry text
      this->isActive = this->entryTextDetection(dataIn);
      break;
    }
  }

  // process input data if menu was already displayed
  if (this->isActive && !this->newMenuToDisplay) {
    this->isActive = this->processInput(dataIn);
    if (!this->isActive) {
      // display welcome screen when just exited menu
      this->display(dataOut);
    }
  }

  // display new menu
  if (this->isActive && this->newMenuToDisplay) {
    this->display(dataOut);
    this->newMenuToDisplay = false;
  }

  return this->isActive;
}

} // namespace app
