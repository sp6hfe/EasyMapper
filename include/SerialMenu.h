#pragma once

#include "Config.h"
#include <Print.h>
#include <pgmspace.h>

namespace app {

#define SERIAL_MENU_SIZE(menu) sizeof(menu) / sizeof(SerialMenuEntry)

enum class SerialMenuEntryType { SUBMENU, BOOLEAN, VALUE };

class SerialMenuEntry {
private:
  const char *description;
  const char actionKey;
  void (*action)();
  const SerialMenuEntryType entryType;
  const void *value;

public:
  const char *getDescription() const { return description; };

  SerialMenuEntry(const char *description_, const char actionKey_,
                  void (*action_)(), const SerialMenuEntryType entryType_,
                  const void *value_)
      : description(description_), actionKey(actionKey_), action(action_),
        entryType(entryType_), value(value_) {}
};

class SerialMenu {
private:
  static constexpr uint8_t ESC_KEY_CODE = 0x1B;
  const uint8_t ENTRY_TRIGGER_TEXT[3] = {'c', 'f', 'g'};
  bool isActive = false;
  bool currentMenuDisplayed = false;

  const SerialMenuEntry *menuEntries;
  uint8_t amountOfMenuEntries;

  void printInfo(Print &dataOut);
  bool entryTriggerDetected(uint8_t dataIn);

public:
  void load(const SerialMenuEntry *menuEntries,
            const uint8_t amountOfMenuEntries);
  void display(Print &dataOut) const;
  bool peform(uint8_t dataIn, Print &dataOut, config_t &config);

  SerialMenu(){};
};

} // namespace app
