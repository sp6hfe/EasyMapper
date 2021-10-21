#pragma once

#include <Arduino.h>
#include <Stream.h>

namespace wrappers {

class HwSerial : public Stream {
private:
  HardwareSerial &serial;

public:
  // Stream
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  // Print
  size_t write(uint8_t c) override;

  HwSerial(HardwareSerial &serial_) : serial(serial_){};
};

int HwSerial::available() { return this->serial.available(); }
int HwSerial::read() { return this->serial.read(); }
int HwSerial::peek() { return this->serial.peek(); }
void HwSerial::flush() { this->serial.flush(); }
size_t HwSerial::write(uint8_t c) { return this->serial.write(c); }

} // namespace wrappers
