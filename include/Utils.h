#pragma once

#include <Stream.h>

namespace utils {

class Numeric {
public:
  enum ToFixedConversion_t {
    LATITUDE,
    LONGTITUDE,
  };

  static uint32_t toFixedPoint(ToFixedConversion_t type, double value);
  static void printDouble(Stream &printer, double value, uint8_t decimalPlaces);
};

uint32_t Numeric::toFixedPoint(Numeric::ToFixedConversion_t type,
                               double value) {
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

void Numeric::printDouble(Stream &printer, double value,
                          uint8_t decimalPlaces) {
  // print integer part
  printer.print(int(value));

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
    printer.print('.');
    while (zeroPaddingCounter--) {
      printer.print('0');
    }
    printer.printf("%d", frac);
  }
}

} // namespace utils
