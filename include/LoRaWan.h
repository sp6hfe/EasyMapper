#pragma once

#include "ILoRaWan.h"
#include "LoRaWanMod.h"

namespace wrappers {

class LoRaWan : public ILoRaWan {
public:
  bool joinOTAA(uint8_t *appEui, uint8_t *appKey,
                uint8_t *devEui = nullptr) override;
  bool joinABP(uint8_t *nwkSKey, uint8_t *appSKey, uint32_t devAddr) override;
  bool isJoined() override;
  bool send(uint8_t datalen, uint8_t *data, uint8_t port,
            bool confirmed) override;
  void setADR(bool enabled) override;
  void setFixedDataRate(int8_t dataRate) override;

  LoRaWan(DeviceClass_t loraWanClass, LoRaMacRegion_t region);
};

} // namespace wrappers
