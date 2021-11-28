#pragma once

// #include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

#define CONSOLE_MENU_SIZE(menu) sizeof(menu) / sizeof(ConsoleMenuEntry)

enum class ConsoleMenuEntryType { SUBMENU, EXIT, NONE, BOOLEAN, VALUE };

class ConsoleMenuEntry {
private:
  const char *description;
  const uint8_t actionKey;
  void (*action)(void *);
  void *actionParameter; // const?
  const ConsoleMenuEntryType entryType;
  bool (*boolValue)(void *);
  void *boolValueParameter; // const?

public:
  const char *getDescription() const { return this->description; };
  const uint8_t getActionKey() const { return this->actionKey; };
  void makeAction() const {
    if (this->action) {
      this->action(actionParameter);
    }
  };
  const ConsoleMenuEntryType getEntryType() const { return entryType; };
  const bool getBoolValue() const {
    if (this->boolValue) {
      return this->boolValue(boolValueParameter);
    }
    return false;
  };

  ConsoleMenuEntry(const char *description_, const uint8_t actionKey_,
                   void (*action_)(void *), void *actionParameter_,
                   const ConsoleMenuEntryType entryType_,
                   bool (*boolValue_)(void *), void *boolValueParameter_)
      : description(description_), actionKey(actionKey_), action(action_),
        actionParameter(actionParameter_), entryType(entryType_),
        boolValue(boolValue_), boolValueParameter(boolValueParameter_) {}
};

class ConsoleMenu {
public:
  static constexpr uint8_t ESC_KEY_CODE = 0x1B;

private:
  const uint8_t ENTRY_TRIGGER_TEXT[3] = {'c', 'f', 'g'};
  bool isActive = false;
  bool newMenuToDisplay = false;

  const ConsoleMenuEntry *menuEntries;
  uint8_t amountOfMenuEntries;

  bool entryTextDetection(uint8_t dataIn);

  void printEntryDescription(Print &dataOut, const char *description,
                             const uint8_t fillToSize) const;
  void printEntryValue(Print &dataOut, const ConsoleMenuEntryType entryType,
                       const bool boolValue, const uint8_t fillToSize) const;
  void display(Print &dataOut) const;
  bool processInput(uint8_t dataIn);

public:
  void load(const ConsoleMenuEntry *menuEntries,
            const uint8_t amountOfMenuEntries);
  bool peform(uint8_t dataIn, Print &dataOut);

  ConsoleMenu(){};
};

} // namespace app
