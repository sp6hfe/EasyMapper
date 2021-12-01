#pragma once

#include "Config.h"
#include "ConsoleMenu.h"
#include "IGps.h"
#include "ILed.h"
#include "ILoRaWan.h"
#include "Stream.h"

namespace app {

class App {
public:
  static constexpr uint8_t PAYLOAD_SIZE = 11;

  // forward declarations used in menu building
  void loadMainMenu();
  void loadPeripheralsMenu();
  void exitMenu();

  static void loadMainMenuWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      thisApp->loadMainMenu();
    }
  };

  static void loadPeripheralsMenuWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      thisApp->loadPeripheralsMenu();
    }
  };

  static bool isLedEnabledWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      return thisApp->led.isEnabled();
    }
    return false;
  };

  static void toggleLedEnableWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      if (thisApp->led.isEnabled()) {
        thisApp->led.disable();
      } else {
        thisApp->led.enable();
      }
      thisApp->loadPeripheralsMenu();
    }
  };

  static bool isGpsEnabledWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      return thisApp->gps.isEnabled();
    }
    return false;
  };

  static void toggleGpsEnableWrapper(void *appClass) {
    if (appClass) {
      App *thisApp = reinterpret_cast<App *>(appClass);
      if (thisApp->gps.isEnabled()) {
        thisApp->gps.disable();
      } else {
        thisApp->gps.enable();
      }
      thisApp->loadPeripheralsMenu();
    }
  };

private:
  // config_t config;
  Stream &console;
  ILoRaWan &lora;
  IGps &gps;
  ILed &led;
  uint8_t payload[PAYLOAD_SIZE];
  IGps::gpsData_t gpsData;

  bool consoleMenuActive = false;
  ConsoleMenu consoleMenu;

  const ConsoleMenuEntry mainMenu[3] = {
      {"  1 Peripherals", '1', loadPeripheralsMenuWrapper,
       reinterpret_cast<void *>(this), ConsoleMenuEntryType::SUBMENU, nullptr,
       nullptr},
      {"  2 LoRa", '2', nullptr, nullptr, ConsoleMenuEntryType::NONE, nullptr,
       nullptr},
      {"ESC Exit configuration", ConsoleMenu::ESC_KEY_CODE, nullptr, nullptr,
       ConsoleMenuEntryType::EXIT, nullptr, nullptr}};
  const uint8_t mainMenuSize = CONSOLE_MENU_SIZE(mainMenu);

  const ConsoleMenuEntry peripheralsMenu[3] = {
      {"  1 LED", '1', toggleLedEnableWrapper, reinterpret_cast<void *>(this),
       ConsoleMenuEntryType::BOOLEAN, isLedEnabledWrapper,
       reinterpret_cast<void *>(this)},
      {"  2 GPS", '2', toggleGpsEnableWrapper, reinterpret_cast<void *>(this),
       ConsoleMenuEntryType::BOOLEAN, isGpsEnabledWrapper,
       reinterpret_cast<void *>(this)},
      {"ESC Main menu", ConsoleMenu::ESC_KEY_CODE, loadMainMenuWrapper,
       reinterpret_cast<void *>(this), ConsoleMenuEntryType::SUBMENU, nullptr,
       nullptr}};
  const uint8_t peripheralsMenuSize = CONSOLE_MENU_SIZE(peripheralsMenu);

  static void preparePayload(const IGps::gpsData_t &data,
                             uint8_t (&payload)[PAYLOAD_SIZE]);

  static uint32_t calculateDelay(const uint32_t lastMillis,
                                 const uint32_t currentMillis);

  void printGpsData();
  void handleConsoleMenu();
  bool handleGps(bool readData);

public:
  void setup();
  void loop(uint32_t loopEnterMillis);

  App(Stream &console_, ILoRaWan &lora_, IGps &gps_, ILed &led_)
      : console(console_), lora(lora_), gps(gps_), led(led_){};
};

} // namespace app