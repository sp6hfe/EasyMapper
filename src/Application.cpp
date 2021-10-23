#include "Application.h"
#include "Utils.h"
#include "secrets.h"

namespace app {

void App::preparePayload(const dataToSend_t &data,
                         uint8_t (&payload)[PAYLOAD_SIZE]) {
  uint8_t index = 0;
  uint32_t gpsLatitudeBinary = utils::Numeric::toFixedPoint(
      utils::Numeric::ToFixedConversion_t::LATITUDE, data.gpsLatitude);
  payload[index++] = (gpsLatitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLatitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLatitudeBinary & 0xFF;
  uint32_t gpsLongitudeBinary = utils::Numeric::toFixedPoint(
      utils::Numeric::ToFixedConversion_t::LONGTITUDE, data.gpsLongtitude);
  payload[index++] = (gpsLongitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLongitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLongitudeBinary & 0xFF;
  uint32_t gpsAltitudeBinary = data.gpsAltitude * 10;
  payload[index++] = (gpsAltitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsAltitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsAltitudeBinary & 0xFF;
  uint32_t gpsHdopBinary = data.gpsHdop * 10;
  payload[index++] = (gpsHdopBinary >> 8) & 0xFF;
  payload[index] = gpsHdopBinary & 0xFF;
}

void App::setup() {
  this->led.setColor(ILed::LED_BLUE);
  this->console->println();
  this->console->print("\t*****************************\n");
  this->console->print("\t* LoRa EasyMapper by SP6HFE *\n");
  this->console->print("\t*****************************\n");
  this->console->print("\nJoining with ABP...\n");
  this->lora->joinABP(secrets::nwkSKey, secrets::appSKey, secrets::devAddr);
  if (this->lora->isJoined()) {
    this->console->print("Joined\n");
    this->led.off();
  } else {
    this->console->print("Join failed\n");
    this->led.setColor(ILed::LED_RED);
  }
}

void App::loop(uint32_t loopEnterMillis) {
  static constexpr uint32_t MIN_READOUT_UPDATE_MS = 15000;
  static uint32_t lastEnterMillis = 0;
  static uint32_t lastSentencesWithFixCount = 0;

  int gps_data = this->gpsLink->read();
  if (gps_data >= 0) {
    gps.encode(static_cast<uint8_t>(gps_data));
  }

  if (loopEnterMillis < lastEnterMillis) {
    lastEnterMillis = 0;
  }

  if (loopEnterMillis - lastEnterMillis >= MIN_READOUT_UPDATE_MS ||
      lastEnterMillis == 0) {
    if (gps.location.isUpdated() || gps.altitude.isUpdated() ||
        gps.hdop.isUpdated()) {
      // gater the data
      bool dataOkToSend = false;
      this->dataToSend.gpsLatitude = gps.location.lat();
      this->dataToSend.gpsLongtitude = gps.location.lng();
      this->dataToSend.gpsAltitude = gps.altitude.meters();
      this->dataToSend.gpsHdop = gps.hdop.hdop();

      // decide if data is worth airing
      if (gps.location.isValid() && this->dataToSend.gpsHdop < 2.0) {
        dataOkToSend = true;
      }

      // print report
      this->console->printf("%02d.%02d.%d %02d:%02d:%02d ", gps.date.day(),
                            gps.date.month(), gps.date.year(), gps.time.hour(),
                            gps.time.minute(), gps.time.second());
      if (dataOkToSend) {
        this->console->print("[+]");
      } else {
        this->console->print("[-]");
      }
      this->console->print(" Lat: ");
      utils::Numeric::printDouble(this->console, this->dataToSend.gpsLatitude,
                                  6);
      this->console->print(", Lon: ");
      utils::Numeric::printDouble(this->console, this->dataToSend.gpsLongtitude,
                                  6);
      this->console->print(", Alt: ");
      utils::Numeric::printDouble(this->console, this->dataToSend.gpsAltitude,
                                  1);
      this->console->print(", HDOP: ");
      utils::Numeric::printDouble(this->console, this->dataToSend.gpsHdop, 1);
      this->console->println();

      // air data
      if (dataOkToSend) {
        preparePayload(this->dataToSend, payload);
        this->led.setColor(ILed::LED_GREEN);
        this->lora->send(sizeof(payload), payload, 1, false);
        this->led.off();
      }

      // keep updates interval
      lastEnterMillis = loopEnterMillis;
    }
  }
}

} // namespace app