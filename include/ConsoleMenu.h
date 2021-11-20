#pragma once

#include "Config.h"
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
  const void *value;

public:
  const char *getDescription() const { return description; };
  const uint8_t getActionKey() const { return actionKey; };
  void makeAction() const {
    if (action) {
      action(actionParameter);
    }
  };
  const ConsoleMenuEntryType getEntryType() const { return entryType; };

  ConsoleMenuEntry(const char *description_, const uint8_t actionKey_,
                   void (*action_)(void *), void *actionParameter_,
                   const ConsoleMenuEntryType entryType_, const void *value_)
      : description(description_), actionKey(actionKey_), action(action_),
        actionParameter(actionParameter_), entryType(entryType_),
        value(value_) {}
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

  uint8_t printEntryValue(Print &dataOut, ConsoleMenuEntryType entryType) const;
  void display(Print &dataOut) const;
  bool processInput(uint8_t dataIn);

public:
  void load(const ConsoleMenuEntry *menuEntries,
            const uint8_t amountOfMenuEntries);
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  ConsoleMenu(){};
};

} // namespace app
