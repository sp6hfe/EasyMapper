#pragma once

#include <Arduino.h>
#include <Stream.h>

namespace wrappers {

class HwSerial : public Stream {
private:
  HardwareSerial *serial;

public:
  void begin(HardwareSerial *serial_) { serial = serial_; };
  // Stream
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  // Print
  size_t write(uint8_t c) override;

  HwSerial(){};
};

int HwSerial::available() {
  if (this->serial) {
    return this->serial->available();
  }
  return 0;
}

int HwSerial::read() {
  if (this->serial) {
    return this->serial->read();
  }
  return (uint32)(-1);
}

int HwSerial::peek() {
  if (this->serial) {
    return this->serial->peek();
  }
  return 0;
}

void HwSerial::flush() {
  if (this->serial) {
    this->serial->flush();
  }
}

size_t HwSerial::write(uint8_t c) {
  if (this->serial) {
    return this->serial->write(c);
  }
  return 1;
}

} // namespace wrappers
