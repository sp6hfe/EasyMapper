#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <softSerial.h>

namespace wrappers {

class SwSerial : public Stream {
public:
  static constexpr uint8_t UNUSED_PIN = 0xFF;

private:
  bool notClosed = false;
  uint8_t txPin;
  uint8_t rxPin;
  uint16_t baudrate = 0;
  softSerial swSerial;

public:
  // Ext device power-off interrupt triggering issue solving
  void close();
  void reOpen();
  // Init
  void begin(uint16_t baudrate);
  // Stream
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  // Print
  size_t write(uint8_t c) override;

  SwSerial(uint8_t txPin_, uint8_t rxPin_)
      : txPin(txPin_), rxPin(rxPin_), swSerial(txPin, rxPin){};
};

void SwSerial::close() {
  if (this->rxPin != UNUSED_PIN) {
    detachInterrupt(this->rxPin);
    pinMode(this->rxPin, OUTPUT);
    digitalWrite(this->rxPin, HIGH);
  }

  if (this->txPin != UNUSED_PIN) {
    pinMode(this->txPin, OUTPUT);
    digitalWrite(this->txPin, HIGH);
  }

  this->notClosed = false;
}

void SwSerial::reOpen() {
  if (this->baudrate) {
    this->begin(this->baudrate);
  }
}

void SwSerial::begin(uint16_t baudrate) {
  // behavior depends on how begin() deals with UNUSED_PINs provided
  swSerial.begin(baudrate);
  this->baudrate = baudrate;
  this->notClosed = true;
}

int SwSerial::available() {
  if (this->notClosed && (this->rxPin != UNUSED_PIN)) {
    return this->swSerial.available();
  }

  return 0;
}

int SwSerial::read() {
  if (this->notClosed && (this->rxPin != UNUSED_PIN)) {
    return this->swSerial.read();
  }
  return (-1);
}

int SwSerial::peek() { return this->swSerial.peek(); }

void SwSerial::flush() { this->swSerial.flush(); }

size_t SwSerial::write(uint8_t c) {
  if (this->notClosed && (this->txPin != UNUSED_PIN)) {
    return this->swSerial.write(c);
  }

  return 1;
}

} // namespace wrappers
