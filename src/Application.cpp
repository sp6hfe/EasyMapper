#include "Application.h"
#include "secrets.h"

namespace app {

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

void App::loop(uint32_t loopEnterMillis) {}

void App::sendLoRaWanData(size_t dataSize, uint8_t *data, uint8_t port,
                          bool ifConfirmed) {
  this->led.setColor(ILed::LED_GREEN);
  this->lora->send(dataSize, data, port, ifConfirmed);
  this->led.off();
}

} // namespace app