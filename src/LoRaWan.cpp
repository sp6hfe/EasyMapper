#include "LoRaWan.h"

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

extern LoRaWanMod LoRaWanModified;

namespace wrappers {

bool LoRaWan::joinOTAA(uint8_t *appEui, uint8_t *appKey, uint8_t *devEui) {
  if (devEui) {
    return LoRaWanModified.joinOTAA(appEui, appKey, devEui);
  }

  return LoRaWanModified.joinOTAA(appEui, appKey);
}

bool LoRaWan::joinABP(uint8_t *nwkSKey, uint8_t *appSKey, uint32_t devAddr) {
  return LoRaWanModified.joinABP(nwkSKey, appSKey, devAddr);
}

bool LoRaWan::isJoined() { LoRaWanModified.isJoined(); }

bool LoRaWan::send(uint8_t datalen, uint8_t *data, uint8_t port,
                   bool confirmed) {
  return LoRaWanModified.send(datalen, data, port, confirmed);
}

void LoRaWan::setADR(bool enabled) { LoRaWanModified.setAdaptiveDR(enabled); }

void LoRaWan::setFixedDataRate(int8_t dataRate) {
  LoRaWanModified.setFixedDR(dataRate);
}

LoRaWan::LoRaWan(DeviceClass_t loraWanClass, LoRaMacRegion_t region) {
  LoRaWanModified.begin(loraWanClass, region);
}

} // namespace wrappers
