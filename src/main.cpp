#include "Application.h"
#include "HwSerial.h"
#include "Led.h"
#include "LoRaWan.h"
#include "SwSerial.h"
#include <Arduino.h>
#include <CubeCell_NeoPixel.h>
#include <softSerial.h>

static constexpr uint8_t VEXT_PIN = GPIO6;
static constexpr uint8_t GPS_RX_PIN = GPIO0;
static constexpr uint8_t GPS_TX_PIN = GPIO5;
static constexpr uint32_t CONSOLE_BAUDRATE = 115200;
static constexpr uint16_t GPS_BAUDRATE = 9600;

static constexpr uint32_t EE_SIZE = 8;

softSerial softwareSerial(GPS_TX_PIN, GPS_RX_PIN);
wrappers::Led led;
wrappers::HwSerial *console;
wrappers::SwSerial *gpsLink;
wrappers::LoRaWan *lora;
app::App *application;

void enableExtPower(bool enable) {
  if (enable) {
    digitalWrite(VEXT_PIN, 0);
    delay(1);
  } else {
    digitalWrite(VEXT_PIN, 1);
  }
}

void setup() {
  // external devices power on
  enableExtPower(true);

  // objects requiring setup
  Serial.begin(CONSOLE_BAUDRATE);
  softwareSerial.begin(GPS_BAUDRATE);

  // wrappers setup (using already available Arduino-instantiated objects)
  console = new wrappers::HwSerial(Serial);
  gpsLink = new wrappers::SwSerial(softwareSerial);
  lora = new wrappers::LoRaWan(DeviceClass_t::CLASS_A,
                               LoRaMacRegion_t::LORAMAC_REGION_EU868);

  // app setup
  application = new app::App(console, gpsLink, lora, led);
  application->setup();
}

void loop() { application->loop(millis()); }