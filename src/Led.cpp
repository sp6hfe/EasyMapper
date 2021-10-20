#include "Led.h"

namespace wrappers {

void Led::on() { this->setColor(this->lastColor); }

void Led::off() { this->setColor(LED_OFF); }

void Led::setColor(uint32_t color) {
  uint8_t red = (uint8_t)(color >> 16);
  uint8_t green = (uint8_t)(color >> 8);
  uint8_t blue = (uint8_t)color;
  this->led.setPixelColor(0, this->led.Color(red, green, blue));
  this->led.show();
  this->lastColor = color;
}

void Led::setBrightness(uint8_t level) { this->led.setBrightness(level); }

} // namespace wrappers