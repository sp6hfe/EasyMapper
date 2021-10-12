#include <Arduino.h>
#include <softSerial.h>
#include <TinyGPS++.h>

static constexpr uint8_t VEXT_PIN = GPIO6;
static constexpr uint8_t GPS_RX_PIN = GPIO0;
static constexpr uint8_t GPS_TX_PIN = GPIO5;
static constexpr uint16_t SW_SERIAL_BAUDRATE = 9600;
static constexpr uint32_t HW_SERIAL_BAUDRATE = 115200;

softSerial ss(GPS_TX_PIN, GPS_RX_PIN);
TinyGPSPlus gps;

void printDouble(double value, uint8_t decimalPlaces)
{
  // print integer part
  Serial.print(int(value));

  // calculate fractional part
  if (decimalPlaces > 0)
  {
    // initial number of zeros if fractional part is really small
    uint8_t zeroPaddingCounter = decimalPlaces - 1;

    // calculate multiplier allowing to extract decimal places
    uint32_t precisionMultiplier = 1;
    while (decimalPlaces--)
    {
      precisionMultiplier *= 10;
    }

    // move fractional part to integers
    uint32_t frac;
    if (value >= 0)
    {
      frac = (value - int(value)) * precisionMultiplier;
    }
    else
    {
      frac = (int(value) - value) * precisionMultiplier;
    }

    // calculate final 0-padding length
    uint32_t fracTemp = frac;
    while (fracTemp /= 10)
    {
      zeroPaddingCounter--;
    }

    //print fractional part
    Serial.print('.');
    while (zeroPaddingCounter--)
    {
      Serial.print('0');
    }
    Serial.printf("%d", frac);
  }
}

void setup()
{
  boardInitMcu();
  ss.begin(SW_SERIAL_BAUDRATE);
  Serial.begin(HW_SERIAL_BAUDRATE);
  Serial.print("\nLoRa versatile node by SP6HFE\n");
  digitalWrite(VEXT_PIN, 0);
}

void loop()
{
  static constexpr uint32_t MIN_READOUT_UPDATE_MS = 5000;
  static uint32_t lastUpdate = 0;
  static uint32_t lastSentencesWithFixCount = 0;

  int gps_data = ss.read();
  if (gps_data >= 0)
  {
    gps.encode(static_cast<uint8_t>(gps_data));
  }

  uint32_t currentTime = millis();
  if (currentTime < lastUpdate)
  {
    lastUpdate = 0;
  }

  if (currentTime - lastUpdate >= MIN_READOUT_UPDATE_MS || lastUpdate == 0)
  {
    if (gps.location.isUpdated() || gps.altitude.isUpdated() || gps.hdop.isUpdated())
    {
      uint32_t sentencesWithFixCount = gps.sentencesWithFix();
      Serial.printf("%02d.%02d.%d %02d:%02d:%02d ", gps.date.day(), gps.date.month(), gps.date.year(), gps.time.hour(), gps.time.minute(), gps.time.second());
      if (sentencesWithFixCount != lastSentencesWithFixCount)
      {
        Serial.print("[+]");
      }
      else
      {
        Serial.print("[-]");
      }
      lastSentencesWithFixCount = sentencesWithFixCount;
      Serial.print(" Lat: ");
      printDouble(gps.location.lat(), 6);
      Serial.print(", Lon: ");
      printDouble(gps.location.lng(), 6);
      Serial.print(", Alt: ");
      printDouble(gps.altitude.meters(), 2);
      Serial.print(", HDOP: ");
      printDouble(gps.hdop.hdop(), 2);
      Serial.println();
      lastUpdate = currentTime;
    }
  }
}