#pragma once

#include "IGps.h"
#include "IGpsPower.h"
#include "SwSerial.h"
#include <TinyGPS++.h>

namespace wrappers {
class Gps : public IGps {
private:
  TinyGPSPlus gps;
  bool enabled = false;
  SwSerial &gpsLink;
  IGpsPower &gpsPower;

public:
  bool process() override;
  bool getData(const uint32_t readoutMillis, gpsData_t &data) override;
  void enable() override;
  void disable() override;
  bool isEnabled() override;

  Gps(SwSerial &gpsLink_, IGpsPower &gpsPower_)
      : gpsLink(gpsLink_), gpsPower(gpsPower_) {
    this->disable();
  };
};

bool Gps::process() {
  if (this->enabled) {
    int gpsData = this->gpsLink.read();
    if (gpsData >= 0) {
      return this->gps.encode(static_cast<uint8_t>(gpsData));
    }
  }

  return false;
}

bool Gps::getData(const uint32_t readoutMillis, gpsData_t &data) {
  bool ifDataUpdated = false;

  if ((this->gps.location.isUpdated() && this->gps.location.isValid()) &&
      (this->gps.altitude.isUpdated() && this->gps.altitude.isValid()) &&
      (this->gps.hdop.isUpdated() && this->gps.hdop.isValid())) {
    data.coordinates.latitude = this->gps.location.lat();
    data.coordinates.longtitude = this->gps.location.lng();
    data.coordinates.altitude = this->gps.altitude.meters();
    data.coordinates.hdop = this->gps.hdop.hdop();
    data.coordinates.updated = true;
    data.coordinates.updateMillis = readoutMillis;
    ifDataUpdated = true;
  }

  if ((this->gps.date.isUpdated() && this->gps.date.isValid()) &&
      (this->gps.time.isUpdated() && this->gps.time.isValid())) {
    data.time.year = this->gps.date.year();
    data.time.month = this->gps.date.month();
    data.time.day = this->gps.date.day();
    data.time.hour = this->gps.time.hour();
    data.time.minute = this->gps.time.minute();
    data.time.second = this->gps.time.second();
    data.time.updated = true;
    data.time.updateMillis = readoutMillis;
    ifDataUpdated = true;
  }

  if (this->gps.satellites.isUpdated() && this->gps.satellites.isValid()) {
    data.reception.satellites =
        static_cast<uint8_t>(this->gps.satellites.value());
    data.reception.updated = true;
    data.reception.updateMillis = readoutMillis;
    ifDataUpdated = true;
  }

  return ifDataUpdated;
}

void Gps::enable() {
  this->gpsPower.gpsPowerOn();
  // serial link re-opening here as GPS pins garbage might cause interrupts
  this->gpsLink.reOpen();
  this->enabled = true;
}

void Gps::disable() {
  // serial link to be closed due to interrupts
  this->gpsLink.close();
  this->gpsPower.gpsPowerOff();
  this->enabled = false;
}

bool Gps::isEnabled() { return this->enabled; }

} // namespace wrappers
