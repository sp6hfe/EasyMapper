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

void App::setup() {
  // handle power automatically when config @ EE will be ready
  this->led.enable();
  this->gps.enable();

  this->led.setColor(ILed::LED_BLUE);
  this->console.println();
  this->console.print("\t*****************************\n");
  this->console.print("\t* LoRa EasyMapper by SP6HFE *\n");
  this->console.print("\t*****************************\n");
  this->console.print("\nJoining with ABP...\n");
  // this->lora.setFixedDataRate();
  this->lora.joinABP(secrets::nwkSKey, secrets::appSKey, secrets::devAddr);
  if (this->lora.isJoined()) {
    this->console.print("Joined\n");
    this->led.off();
  } else {
    this->console.print("Join failed\n");
    // TODO: handle this situation
    this->led.setColor(ILed::LED_RED);
  }
}

void App::loop(uint32_t loopEnterMillis) {
  static constexpr uint32_t MIN_READOUT_UPDATE_MS = 15000;
  static uint32_t lastEnterMillis = 0;
  static uint32_t lastSentencesWithFixCount = 0;

  // TODO: handle returned value
  this->gps.process();

  if (loopEnterMillis < lastEnterMillis) {
    lastEnterMillis = 0;
  }

  if (loopEnterMillis - lastEnterMillis >= MIN_READOUT_UPDATE_MS ||
      lastEnterMillis == 0) {
    if (this->gps.isDataUpdated()) {
      // gater the data
      this->gps.getData(this->gpsData);
      // decide if data is worth airing
      bool dataOkToSend = false;
      if (this->gps.isDataValid()) {
        dataOkToSend = true;
      }

      // print report
      this->console.printf("%02d.%02d.%d %02d:%02d:%02d ",
                           this->gpsData.time.day, this->gpsData.time.month,
                           this->gpsData.time.year, this->gpsData.time.hour,
                           this->gpsData.time.minute,
                           this->gpsData.time.second);
      if (dataOkToSend) {
        this->console.print("[+]");
      } else {
        this->console.print("[-]");
      }
      this->console.print(" Lat: ");
      utils::Numeric::printDouble(this->console,
                                  this->gpsData.coordinates.latitude, 6);
      this->console.print(", Lon: ");
      utils::Numeric::printDouble(this->console,
                                  this->gpsData.coordinates.longtitude, 6);
      this->console.print(", Alt: ");
      utils::Numeric::printDouble(this->console,
                                  this->gpsData.coordinates.altitude, 1);
      this->console.print(", HDOP: ");
      utils::Numeric::printDouble(this->console, this->gpsData.coordinates.hdop,
                                  1);
      this->console.println();

      // air data
      if (dataOkToSend) {
        preparePayload(this->gpsData, payload);
        this->led.setColor(ILed::LED_GREEN);
        this->lora.send(sizeof(payload), payload, 1, false);
        this->led.off();
      }

      // keep updates interval
      lastEnterMillis = loopEnterMillis;
    }
  }
}

} // namespace app