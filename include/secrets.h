#pragma once

#include <stdint.h>

namespace secrets {

// OTAA parameters
uint8_t devEui[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
uint8_t appEui[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
uint8_t appKey[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                    0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

// ABP parameters
uint8_t nwkSKey[] = {0xF9, 0x94, 0xFB, 0x81, 0x0C, 0xE9, 0x53, 0x55,
                     0xE3, 0xB3, 0xEC, 0x30, 0xAF, 0x92, 0xB0, 0xF9};
uint8_t appSKey[] = {0xF0, 0x47, 0xFD, 0x33, 0xFD, 0x99, 0x45, 0x88,
                     0x6C, 0xE5, 0xE4, 0x09, 0x49, 0xA4, 0x51, 0x56};
uint32_t devAddr = (uint32_t)0x260B46AE;

} // namespace secrets
