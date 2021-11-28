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
  if (this->gpsData.isValid) {
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

bool App::handleGps(bool readData) {
  bool ifNewGpsDataRead = false;

  if (this->gps.process() && this->gps.isDataUpdated() && readData) {
    this->gps.getData(this->gpsData);
    ifNewGpsDataRead = true;
  }

  return ifNewGpsDataRead;
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

void App::loop(uint32_t loopEnterMillis) {
  static constexpr uint32_t GPS_READOUT_INTERVAL_MS = 15000;
  static uint32_t lastMillisOnGpsDataRead = 0;

  this->handleConsoleMenu();

  if (!this->consoleMenuActive) {
    uint32_t delaySinceGpsDataRead =
        this->calculateDelay(lastMillisOnGpsDataRead, loopEnterMillis);
    bool gpsDataReadFlag =
        (delaySinceGpsDataRead >= GPS_READOUT_INTERVAL_MS) ? true : false;

    bool newGpsDataRead = this->handleGps(gpsDataReadFlag);
    if (newGpsDataRead) {
      this->printGpsData();
    }

    if (gpsDataReadFlag) {
      // mark GPS data read was just requested (despite the fact if it was
      // valid to read)
      lastMillisOnGpsDataRead = loopEnterMillis;
    }

    if (newGpsDataRead && this->gpsData.isValid) {
      // air GPS data
      preparePayload(this->gpsData, payload);
      this->led.setColor(ILed::LED_GREEN);
      this->lora.send(sizeof(payload), payload, 1, false);
      this->led.off();
    }
  }
}

} // namespace app