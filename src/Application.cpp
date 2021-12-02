#include "Application.h"
#include "Utils.h"
#include "secrets.h"

namespace app {

void App::preparePayload(const IGps::gpsData_t &data,
                         uint8_t (&payload)[PAYLOAD_SIZE]) {
  uint8_t index = 0;
  uint32_t gpsLatitudeBinary = utils::Numeric::toFixedPoint(
      utils::Numeric::ToFixedConversion_t::LATITUDE, data.coordinates.latitude);
  payload[index++] = (gpsLatitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLatitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLatitudeBinary & 0xFF;
  uint32_t gpsLongitudeBinary = utils::Numeric::toFixedPoint(
      utils::Numeric::ToFixedConversion_t::LONGTITUDE,
      data.coordinates.longtitude);
  payload[index++] = (gpsLongitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLongitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLongitudeBinary & 0xFF;
  uint32_t gpsAltitudeBinary = data.coordinates.altitude * 10;
  payload[index++] = (gpsAltitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsAltitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsAltitudeBinary & 0xFF;
  uint32_t gpsHdopBinary = data.coordinates.hdop * 10;
  payload[index++] = (gpsHdopBinary >> 8) & 0xFF;
  payload[index] = gpsHdopBinary & 0xFF;
}

uint32_t App::calculateDelay(const uint32_t lastMillis,
                             const uint32_t currentMillis) {
  uint32_t delay = 0;

  if (lastMillis < currentMillis) {
    delay = currentMillis - lastMillis;
  } else {
    // millis counter rolled-over 0 and we assume delay was shorter than millis
    // counter capacity
    delay = (0xFFFFFFFF - lastMillis) + currentMillis + 1;
  }

  return delay;
}

void App::printGpsData() {
  this->console.printf("%02d.%02d.%d %02d:%02d:%02d ", this->gpsData.time.day,
                       this->gpsData.time.month, this->gpsData.time.year,
                       this->gpsData.time.hour, this->gpsData.time.minute,
                       this->gpsData.time.second);
  if (this->isGpsDataUpdated()) {
    this->console.print("[+]");
  } else {
    this->console.print("[-]");
  }
  this->console.print(" Lat: ");
  utils::Numeric::printDouble(this->console, this->gpsData.coordinates.latitude,
                              6);
  this->console.print(", Lon: ");
  utils::Numeric::printDouble(this->console,
                              this->gpsData.coordinates.longtitude, 6);
  this->console.print(", Alt: ");
  utils::Numeric::printDouble(this->console, this->gpsData.coordinates.altitude,
                              1);
  this->console.print(", HDOP: ");
  utils::Numeric::printDouble(this->console, this->gpsData.coordinates.hdop, 1);
  this->console.println();
}

void App::markGpsDataAsProcessed() {
  this->gpsData.coordinates.updated = false;
  this->gpsData.time.updated = false;
  this->gpsData.reception.updated = false;
}

bool App::isGpsDataUpdated() {
  return (this->gpsData.coordinates.updated || this->gpsData.time.updated ||
          this->gpsData.reception.updated);
}

void App::handleConsoleMenu() {
  int consoleDataIn = this->console.read();

  if (consoleDataIn >= 0) {
    if (!this->consoleMenuActive) {
      // set default menu on enter
      this->loadMainMenu();
    }
    this->consoleMenuActive = this->consoleMenu.peform(
        static_cast<uint8_t>(consoleDataIn), this->console);
  }
}

bool App::handleGps(const uint32_t currentMillis, const bool acquireData) {
  bool ifNewGpsDataAcquired = false;

  if (this->gps.process() && acquireData &&
      this->gps.getData(currentMillis, this->gpsData)) {
    ifNewGpsDataAcquired = true;
  }

  return ifNewGpsDataAcquired;
}

void App::loadMainMenu() {
  this->consoleMenu.load(this->mainMenu, this->mainMenuSize);
}

void App::loadPeripheralsMenu() {
  this->consoleMenu.load(this->peripheralsMenu, this->peripheralsMenuSize);
}

void App::setup() {
  // handle power automatically when config @ EE will be ready
  this->led.enable();
  this->gps.enable();

  this->led.setColor(ILed::LED_BLUE);
  this->console.println("\nJoining with ABP...");
  // this->lora.setFixedDataRate();
  this->lora.joinABP(secrets::nwkSKey, secrets::appSKey, secrets::devAddr);
  if (this->lora.isJoined()) {
    this->console.println("Joined.");
    this->led.off();
  } else {
    this->console.println("Join failed.");
    // TODO: handle this situation
    this->led.setColor(ILed::LED_RED);
  }
}

void App::loop(const uint32_t loopEnterMillis) {
  static constexpr uint32_t GPS_DATA_CACHE_INTERVAL_MS = 15000;
  static uint32_t lastMillisOnGpsDataCache = 0;

  this->handleConsoleMenu();

  if (!this->consoleMenuActive) {
    bool acquireGpsData =
        (this->calculateDelay(lastMillisOnGpsDataCache, loopEnterMillis) >=
         GPS_DATA_CACHE_INTERVAL_MS)
            ? true
            : false;

    // GPS data parsed constantly when not in menu
    bool newGpsDataAcquired = this->handleGps(loopEnterMillis, acquireGpsData);

    if (acquireGpsData) {
      this->printGpsData();
      if (newGpsDataAcquired) {
        // air GPS data
        preparePayload(this->gpsData, payload);
        this->led.setColor(ILed::LED_GREEN);
        this->lora.send(sizeof(payload), payload, 1, false);
        this->led.off();
      }
      this->markGpsDataAsProcessed();

      // mark GPS data chache was just requested
      // regardless if parsed data was valid
      lastMillisOnGpsDataCache = loopEnterMillis;
    }
  }
}

} // namespace app