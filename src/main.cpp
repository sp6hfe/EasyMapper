#include "Application.h"
#include "Gps.h"
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
  // TODO: let it be handled by application
  enableExtPower(true);
}

void loop() {
  // TODO: figure if setup may be used for init
  softSerial softwareSerial(GPS_TX_PIN, GPS_RX_PIN);
  wrappers::HwSerial console;
  wrappers::SwSerial gpsLink;
  wrappers::Led led;
  wrappers::Gps gps;
  wrappers::LoRaWan lora(DeviceClass_t::CLASS_A,
                         LoRaMacRegion_t::LORAMAC_REGION_EU868);

  Serial.begin(CONSOLE_BAUDRATE);
  console.begin(&Serial);
  softwareSerial.begin(GPS_BAUDRATE);
  gpsLink.begin(&softwareSerial);
  gps.begin(&gpsLink);

  app::App application(console, lora, gps, led);
  application.setup();

  while (1) {
    application.loop(millis());
  }
}