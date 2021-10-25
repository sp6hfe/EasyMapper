#pragma once

#include "IGps.h"
#include <Stream.h>
#include <TinyGPS++.h>

namespace wrappers {
class Gps : public IGps {
private:
  Stream *gpsLink;
  TinyGPSPlus gps;

public:
  void begin(Stream *gpsLink_) { gpsLink = gpsLink_; };
  bool process() override;
  bool isDataUpdated() override;
  bool isDataValid() override;
  void getData(gpsData_t &data) override;

  Gps(){};
};

bool Gps::process() {
  if (this->gpsLink) {
    int gpsData = this->gpsLink->read();
    if (gpsData >= 0) {
      return this->gps.encode(static_cast<uint8_t>(gpsData));
    }
  }
  return false;
}

bool Gps::isDataUpdated() {
  return (this->gps.location.isUpdated() || this->gps.altitude.isUpdated() ||
          this->gps.hdop.isUpdated());
}

bool Gps::isDataValid() {
  return (this->gps.location.isValid() && this->gps.altitude.isValid() &&
          this->gps.hdop.isValid());
}

void Gps::getData(gpsData_t &data) {
  data.coordinates.latitude = this->gps.location.lat();
  data.coordinates.longtitude = this->gps.location.lng();
  data.coordinates.altitude = this->gps.altitude.meters();
  data.coordinates.hdop = this->gps.hdop.hdop();
  data.time.year = this->gps.date.year();
  data.time.month = this->gps.date.month();
  data.time.day = this->gps.date.day();
  data.time.hour = this->gps.time.hour();
  data.time.minute = this->gps.time.minute();
  data.time.second = this->gps.time.second();
}
} // namespace wrappers
