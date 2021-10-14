#include <Arduino.h>
#include <softSerial.h>
#include <TinyGPS++.h>
#include <LoRaWanMinimal_APP.h>
#include <CubeCell_NeoPixel.h>

/* OTAA para*/
uint8_t devEui[] = {0x22, 0x32, 0x33, 0x00, 0x00, 0x88, 0x88, 0x02};
uint8_t appEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t appKey[] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x66, 0x01};

/* ABP para*/
uint8_t nwkSKey[] = {0xF9, 0x94, 0xFB, 0x81, 0x0C, 0xE9, 0x53, 0x55, 0xE3, 0xB3, 0xEC, 0x30, 0xAF, 0x92, 0xB0, 0xF9};
uint8_t appSKey[] = {0xF0, 0x47, 0xFD, 0x33, 0xFD, 0x99, 0x45, 0x88, 0x6C, 0xE5, 0xE4, 0x09, 0x49, 0xA4, 0x51, 0x56};
uint32_t devAddr = (uint32_t)0x260B46AE;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

static constexpr uint32_t LED_OFF = 0x00000000;
static constexpr uint32_t LED_RED = 0x00FF0000;
static constexpr uint32_t LED_GREEN = 0x0000FF00;
static constexpr uint32_t LED_BLUE = 0x000000FF;

static constexpr uint8_t VEXT_PIN = GPIO6;
static constexpr uint8_t GPS_RX_PIN = GPIO0;
static constexpr uint8_t GPS_TX_PIN = GPIO5;
static constexpr uint16_t SW_SERIAL_BAUDRATE = 9600;
static constexpr uint32_t HW_SERIAL_BAUDRATE = 115200;

softSerial ss(GPS_TX_PIN, GPS_RX_PIN);
TinyGPSPlus gps;
CubeCell_NeoPixel led(1, RGB, NEO_GRB + NEO_KHZ800);

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

void enableExtPower(bool enable)
{
  if (enable)
  {
    digitalWrite(VEXT_PIN, 0);
    delay(1);
  }
  else
  {
    digitalWrite(VEXT_PIN, 1);
  }
}

void ledInit(CubeCell_NeoPixel *led)
{
  if (led)
  {
    led->begin();
    led->clear();
    led->setBrightness(10);
  }
}

void ledSetColor(CubeCell_NeoPixel *led, uint32_t color)
{
  if (led)
  {
    uint8_t red = (uint8_t)(color >> 16);
    uint8_t green = (uint8_t)(color >> 8);
    uint8_t blue = (uint8_t)color;
    led->setPixelColor(0, led->Color(red, green, blue));
    led->show();
  }
}

void setup()
{
  enableExtPower(true);
  ledInit(&led);
  ledSetColor(&led, LED_BLUE);
  ss.begin(SW_SERIAL_BAUDRATE);
  Serial.begin(HW_SERIAL_BAUDRATE);
  Serial.print("\nLoRa versatile node by SP6HFE\n");

  LoRaWAN.begin(DeviceClass_t::CLASS_A, LoRaMacRegion_t::LORAMAC_REGION_EU868);
  LoRaWAN.joinABP(nwkSKey, appSKey, devAddr);
  ledSetColor(&led, LED_OFF);
}

void loop()
{
  static constexpr uint32_t MIN_READOUT_UPDATE_MS = 10000;
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

      uint8_t payload[4] = {'A', 'B', 'C', 'D'};
      ledSetColor(&led, LED_GREEN);
      LoRaWAN.send(sizeof(payload), payload, 1, false);
      ledSetColor(&led, LED_OFF);
    }
  }
}