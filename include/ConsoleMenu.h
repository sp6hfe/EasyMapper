#pragma once

#include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

#define CONSOLE_MENU_SIZE(menu) sizeof(menu) / sizeof(ConsoleMenuEntry)

enum class ConsoleMenuEntryType { SUBMENU, BOOLEAN, VALUE };

class ConsoleMenuEntry {
private:
  const char *description;
  const char actionKey;
  void (*action)(void *);
  const ConsoleMenuEntryType entryType;
  const void *value;

public:
  const char *getDescription() const { return description; };

  ConsoleMenuEntry(const char *description_, const char actionKey_,
                   void (*action_)(void *),
                   const ConsoleMenuEntryType entryType_, const void *value_)
      : description(description_), actionKey(actionKey_), action(action_),
        entryType(entryType_), value(value_) {}
};

class ConsoleMenu {
private:
  static constexpr uint8_t ESC_KEY_CODE = 0x1B;
  const uint8_t ENTRY_TRIGGER_TEXT[3] = {'c', 'f', 'g'};
  bool isActive = false;
  bool currentMenuDisplayed = false;

  const ConsoleMenuEntry *menuEntries;
  uint8_t amountOfMenuEntries;

  void printInfo(Print &dataOut);
  bool entryTriggerDetected(uint8_t dataIn);

public:
  void load(const ConsoleMenuEntry *menuEntries,
            const uint8_t amountOfMenuEntries);
  void display(Print &dataOut) const;
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  ConsoleMenu(){};
};

} // namespace app
