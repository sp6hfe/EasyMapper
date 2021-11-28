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

void ConsoleMenu::printEntryDescription(Print &dataOut, const char *description,
                                        const uint8_t displaySize) const {
  uint8_t lettersPrinted = dataOut.print(description);
  for (; lettersPrinted < displaySize; lettersPrinted++) {
    dataOut.print(" ");
  }
}

void ConsoleMenu::printEntryValue(Print &dataOut,
                                  const ConsoleMenuEntryType entryType,
                                  const bool boolValue,
                                  const uint8_t fillToSize) const {
  uint8_t lettersPrinted = 0;

  switch (entryType) {
  case ConsoleMenuEntryType::SUBMENU:
    lettersPrinted += dataOut.print("[...]");
    break;
  case ConsoleMenuEntryType::EXIT:
    lettersPrinted += dataOut.print("[xxx]");
    break;
  case ConsoleMenuEntryType::BOOLEAN:
    if (boolValue) {
      lettersPrinted += dataOut.print("[ENA]");
    } else {
      lettersPrinted += dataOut.print("[OFF]");
    }
    break;
  default:
    break;
  }

  for (; lettersPrinted < fillToSize; lettersPrinted++) {
    dataOut.print(" ");
  }
}

void ConsoleMenu::display(Print &dataOut) const {
  static constexpr uint8_t MENU_DESCRIPTION_SECTION_WIDE = 25;
  static constexpr uint8_t MENU_VALUES_SECTION_WIDE = 5;

  if (this->isActive) {
    dataOut.println("+-----------------------------------+");
    dataOut.println("| Select an option or ESC to return |");
    dataOut.println("+-----------------------------------+");

    if (this->newMenuToDisplay) {
      if (this->menuEntries) {
        for (uint8_t index; index < this->amountOfMenuEntries; index++) {
          // leading bar with separator
          dataOut.print("| ");
          // menu entry description + fill
          this->printEntryDescription(dataOut,
                                      this->menuEntries[index].getDescription(),
                                      MENU_DESCRIPTION_SECTION_WIDE);
          // middle separator
          dataOut.print(" | ");
          // value part + fill
          this->printEntryValue(dataOut,
                                this->menuEntries[index].getEntryType(),
                                this->menuEntries[index].getBoolValue(),
                                MENU_VALUES_SECTION_WIDE);
          // final bar with separator
          dataOut.println(" |");
        }
      } else { // if (this->menuEntries) {
        dataOut.println("|  no options - menu misconfigured  |");
      }
    } else { // if (this->newMenuToDisplay) {
      dataOut.println("|  no options - menu wasn't loaded  |");
    }
    dataOut.println("+-----------------------------------+");
  } else { // if (this->isActive) {
    // initial screen displayed when menu is not yet activated
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
        this->menuEntries[index].makeAction();
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

bool ConsoleMenu::peform(uint8_t dataIn, Print &dataOut) {
  if (!this->isActive) {
    switch (dataIn) {
    case '\n':
      // ENTER code is 0x0D0A so here we have already processed 0x0D by
      // entryTextDetection
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
