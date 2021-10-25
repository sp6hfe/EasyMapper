#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <softSerial.h>

namespace wrappers {

class SwSerial : public Stream {
private:
  softSerial *serial;

public:
  void begin(softSerial *serial_) { serial = serial_; };
  // Stream
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  // Print
  size_t write(uint8_t c) override;

  SwSerial(){};
};

int SwSerial::available() {
  if (this->serial) {
    return this->serial->available();
  }
  return 0;
}

int SwSerial::read() {
  if (this->serial) {
    return this->serial->read();
  }
  return (uint32)(-1);
}

int SwSerial::peek() {
  if (this->serial) {
    return this->serial->peek();
  }
  return 0;
}

void SwSerial::flush() {
  if (this->serial) {
    this->serial->flush();
  }
}

size_t SwSerial::write(uint8_t c) {
  if (this->serial) {
    return this->serial->write(c);
  }
  return 1;
}

} // namespace wrappers
