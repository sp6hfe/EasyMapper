#include "Application.h"
#include "Led.h"
#include "secrets.hpp"
#include <Arduino.h>
#include <CubeCell_NeoPixel.h>
#include <LoRaWanMinimal_APP.h>
#include <TinyGPS++.h>
#include <softSerial.h>

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

static constexpr uint8_t VEXT_PIN = GPIO6;
static constexpr uint8_t GPS_RX_PIN = GPIO0;
static constexpr uint8_t GPS_TX_PIN = GPIO5;
static constexpr uint16_t SW_SERIAL_BAUDRATE = 9600;
static constexpr uint32_t HW_SERIAL_BAUDRATE = 115200;

static constexpr uint8_t PAYLOAD_SIZE = 11;
static constexpr uint32_t EE_SIZE = 8;

softSerial ss(GPS_TX_PIN, GPS_RX_PIN);
TinyGPSPlus gps;

CubeCell_NeoPixel _led(1, RGB, NEO_GRB + NEO_KHZ800);
wrappers::Led led(_led);

uint8_t payload[PAYLOAD_SIZE];

app::App _app(led);

void printDouble(double value, uint8_t decimalPlaces) {
  // print integer part
  Serial.print(int(value));

  // calculate fractional part
  if (decimalPlaces > 0) {
    // initial number of zeros if fractional part is really small
    uint8_t zeroPaddingCounter = decimalPlaces - 1;

    // calculate multiplier allowing to extract decimal places
    uint32_t precisionMultiplier = 1;
    while (decimalPlaces--) {
      precisionMultiplier *= 10;
    }

    // move fractional part to integers
    uint32_t frac;
    if (value >= 0) {
      frac = (value - int(value)) * precisionMultiplier;
    } else {
      frac = (int(value) - value) * precisionMultiplier;
    }

    // calculate final 0-padding length
    uint32_t fracTemp = frac;
    while (fracTemp /= 10) {
      zeroPaddingCounter--;
    }

    // print fractional part
    Serial.print('.');
    while (zeroPaddingCounter--) {
      Serial.print('0');
    }
    Serial.printf("%d", frac);
  }
}

void enableExtPower(bool enable) {
  if (enable) {
    digitalWrite(VEXT_PIN, 0);
    delay(1);
  } else {
    digitalWrite(VEXT_PIN, 1);
  }
}

enum ToFixedConversion_t {
  LATITUDE,
  LONGTITUDE,
};

uint32_t toFixedPoint(ToFixedConversion_t type, double value) {
  // scaling and left shifting to get fixed point
  uint32_t fixedPointValue = 0;

  // scale <0, 1>
  switch (type) {
  case LATITUDE:
    // angle range -90, +90
    value += 90;
    value /= 180;
    break;
  case LONGTITUDE:
    // angle range -180, +180
    value += 180;
    value /= 360;
    break;
  default:
    break;
  }

  // scaled value turned into 16.16 fixed point
  fixedPointValue = static_cast<uint32_t>(value * 16777215);

  return fixedPointValue;
}

struct dataToSend_t {
  double gpsLatitude;
  double gpsLongtitude;
  double gpsAltitude;
  double gpsHdop;
} dataToSend;

void preparePayload(const dataToSend_t &data,
                    uint8_t (&payload)[PAYLOAD_SIZE]) {
  uint8_t index = 0;
  uint32_t gpsLatitudeBinary = toFixedPoint(LATITUDE, data.gpsLatitude);
  payload[index++] = (gpsLatitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLatitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLatitudeBinary & 0xFF;
  uint32_t gpsLongitudeBinary = toFixedPoint(LONGTITUDE, data.gpsLongtitude);
  payload[index++] = (gpsLongitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsLongitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsLongitudeBinary & 0xFF;
  uint32_t gpsAltitudeBinary = data.gpsAltitude * 10;
  payload[index++] = (gpsAltitudeBinary >> 16) & 0xFF;
  payload[index++] = (gpsAltitudeBinary >> 8) & 0xFF;
  payload[index++] = gpsAltitudeBinary & 0xFF;
  uint32_t gpsHdopBinary = data.gpsHdop * 10;
  payload[index++] = (gpsHdopBinary >> 8) & 0xFF;
  payload[index] = gpsHdopBinary & 0xFF;
}

void setup() {
  enableExtPower(true);

  _app.init();

  Serial.begin(HW_SERIAL_BAUDRATE);
  Serial.print("\nLoRa EasyMapper by SP6HFE\n");
  ss.begin(SW_SERIAL_BAUDRATE);

  LoRaWAN.begin(DeviceClass_t::CLASS_A, LoRaMacRegion_t::LORAMAC_REGION_EU868);
  LoRaWAN.joinABP(nwkSKey, appSKey, devAddr);

  // TODO: remove when app will handle that
  led.off();
}

void loop() {
  static constexpr uint32_t MIN_READOUT_UPDATE_MS = 15000;
  static uint32_t lastUpdate = 0;
  static uint32_t lastSentencesWithFixCount = 0;

  int gps_data = ss.read();
  if (gps_data >= 0) {
    gps.encode(static_cast<uint8_t>(gps_data));
  }

  uint32_t currentTime = millis();
  if (currentTime < lastUpdate) {
    lastUpdate = 0;
  }

  if (currentTime - lastUpdate >= MIN_READOUT_UPDATE_MS || lastUpdate == 0) {
    if (gps.location.isUpdated() || gps.altitude.isUpdated() ||
        gps.hdop.isUpdated()) {
      // gater the data
      bool dataOkToSend = false;
      dataToSend.gpsLatitude = gps.location.lat();
      dataToSend.gpsLongtitude = gps.location.lng();
      dataToSend.gpsAltitude = gps.altitude.meters();
      dataToSend.gpsHdop = gps.hdop.hdop();

      // decide if data is worth airing
      if (gps.location.isValid() && dataToSend.gpsHdop < 2.0) {
        dataOkToSend = true;
      }

      // print report
      Serial.printf("%02d.%02d.%d %02d:%02d:%02d ", gps.date.day(),
                    gps.date.month(), gps.date.year(), gps.time.hour(),
                    gps.time.minute(), gps.time.second());
      if (dataOkToSend) {
        Serial.print("[+]");
      } else {
        Serial.print("[-]");
      }
      Serial.print(" Lat: ");
      printDouble(dataToSend.gpsLatitude, 6);
      Serial.print(", Lon: ");
      printDouble(dataToSend.gpsLongtitude, 6);
      Serial.print(", Alt: ");
      printDouble(dataToSend.gpsAltitude, 1);
      Serial.print(", HDOP: ");
      printDouble(dataToSend.gpsHdop, 1);
      Serial.println();

      // air data
      if (dataOkToSend) {
        led.setColor(ILed::LED_GREEN);
        led.on();
        preparePayload(dataToSend, payload);
        LoRaWAN.send(sizeof(payload), payload, 1, false);
        led.off();
      }

      // keep updates interval
      lastUpdate = currentTime;
    }
  }

  _app.run();
}