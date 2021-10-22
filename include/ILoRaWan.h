#pragma once

#include <stdint.h>

class ILoRaWan {
public:
  virtual bool joinOTAA(uint8_t *appEui, uint8_t *appKey,
                        uint8_t *devEui = nullptr) = 0;
  virtual bool joinABP(uint8_t *nwkSKey, uint8_t *appSKey,
                       uint32_t devAddr) = 0;
  virtual bool isJoined() = 0;
  virtual bool send(uint8_t datalen, uint8_t *data, uint8_t port,
                    bool confirmed) = 0;
  virtual void setADR(bool enabled) = 0;
  virtual void setFixedDataRate(int8_t dataRate) = 0;
};