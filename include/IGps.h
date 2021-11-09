#pragma once

#include "IExtDevicel.h"
#include <stdint.h>

class IGps : public IExtDevice {
public:
  struct gpsTime_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
  };

  struct gpsCoordinates_t {
    double latitude;
    double longtitude;
    double altitude;
    double hdop;
  };

  struct gpsData_t {
    gpsCoordinates_t coordinates;
    gpsTime_t time;
    bool isValid;
  };

  virtual bool process() = 0;
  virtual bool isDataUpdated() = 0;
  virtual bool isDataValid() = 0;
  virtual void getData(gpsData_t &data) = 0;
};
