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

private:
  config_t config;
  Stream &console;
  ILoRaWan &lora;
  IGps &gps;
  ILed &led;
  uint8_t payload[PAYLOAD_SIZE];
  IGps::gpsData_t gpsData;

  ConsoleMenu serialMenu;

  const ConsoleMenuEntry mainMenu[2] = {
      {"1 PERIPHERALS", '1', loadMainMenuWrapper, ConsoleMenuEntryType::SUBMENU,
       nullptr},
      {"2 LORA", '2', nullptr, ConsoleMenuEntryType::SUBMENU, nullptr}};
  const uint8_t mainMenuSize = CONSOLE_MENU_SIZE(mainMenu);

  const ConsoleMenuEntry peripheralsMenu[2] = {
      {"1 LED", '1', nullptr, ConsoleMenuEntryType::BOOLEAN,
       &config.ledEnabled},
      {"2 GPS", '2', nullptr, ConsoleMenuEntryType::BOOLEAN,
       &config.gpsEnabled}};
  const uint8_t peripheralsMenuSize = CONSOLE_MENU_SIZE(peripheralsMenu);

  static void preparePayload(const IGps::gpsData_t &data,
                             uint8_t (&payload)[PAYLOAD_SIZE]);

public:
  void setup();
  void loop(uint32_t loopEnterMillis);

  App(Stream &console_, ILoRaWan &lora_, IGps &gps_, ILed &led_)
      : console(console_), lora(lora_), gps(gps_), led(led_){};
};

} // namespace app