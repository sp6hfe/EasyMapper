#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <softSerial.h>

namespace wrappers {

class SwSerial : public Stream {
private:
  softSerial &serial;

public:
  // Stream
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  // Print
  size_t write(uint8_t c) override;

  SwSerial(softSerial &serial_) : serial(serial_){};
};

int SwSerial::available() { return this->serial.available(); }
int SwSerial::read() { return this->serial.read(); }
int SwSerial::peek() { return this->serial.peek(); }
void SwSerial::flush() { this->serial.flush(); }
size_t SwSerial::write(uint8_t c) { return this->serial.write(c); }

} // namespace wrappers
