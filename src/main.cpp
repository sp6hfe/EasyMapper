#include "Application.h"
#include "ExtPowerManager.h"
#include "Gps.h"
#include "HwSerial.h"
#include "Led.h"
#include "LoRaWan.h"
#include "SwSerial.h"
#include <Arduino.h>
#include <CubeCell_NeoPixel.h>

static constexpr uint8_t VEXT_PIN = GPIO6;
static constexpr uint8_t GPS_RX_PIN = GPIO0;
static constexpr uint8_t GPS_TX_PIN = GPIO5;
static constexpr uint32_t CONSOLE_BAUDRATE = 115200;
static constexpr uint16_t GPS_BAUDRATE = 9600;
static constexpr uint32_t EE_SIZE = 8;

app::App *application;

void setup() { Serial.begin(CONSOLE_BAUDRATE); }

void loop() {
  // TODO: figure if setup may be used for init
  wrappers::HwSerial console;
  console.begin(&Serial);

  pwr::ExtPower extPower;
  extPower.begin(VEXT_PIN);
  wrappers::Led led(extPower);

  wrappers::SwSerial gpsLink(GPS_TX_PIN, GPS_RX_PIN);
  gpsLink.begin(GPS_BAUDRATE);

  wrappers::Gps gps(extPower);
  gps.begin(&gpsLink);

  wrappers::LoRaWan lora(DeviceClass_t::CLASS_A,
                         LoRaMacRegion_t::LORAMAC_REGION_EU868);

  app::App application(console, lora, gps, led);
  application.setup();

  for (;;) {
    application.loop(millis());
  }
}