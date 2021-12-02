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
    bool updated;
    uint32_t updateMillis;
  };

  struct gpsCoordinates_t {
    double latitude;
    double longtitude;
    double altitude;
    double hdop;
    bool updated;
    uint32_t updateMillis;
  };

  struct gpsReception_t {
    uint8_t satellites;
    bool updated;
    uint32_t updateMillis;
  };

  struct gpsData_t {
    gpsCoordinates_t coordinates;
    gpsTime_t time;
    gpsReception_t reception;
  };

  virtual bool process() = 0;
  virtual bool getData(const uint32_t readoutMillis, gpsData_t &data) = 0;
};
