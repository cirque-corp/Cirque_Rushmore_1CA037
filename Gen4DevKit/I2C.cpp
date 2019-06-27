// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <stdint.h>
#include <Wire.h>
#include <utility\twi.h>
#include "I2C.h"

#if BUFFER_LENGTH < 53
#error BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\arduino\avr\libraries\Wire\src\Wire.h
#error BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire\Wire.h
#error BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire\WireIMXRT.h
#error BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire\WireKinetis.h
#endif

#if TWI_BUFFER_LENGTH < 53
#error TWI_BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\arduino\avr\libraries\Wire\src\utility\twi.h
#error TWI_BUFFER_LENGTH must be at least 53 for I2C_HID serial to work correctly. Go to \Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Wire\utility\twi.h
#endif

void I2C_init(uint32_t clockFrequency)
{
  Wire.begin();
  Wire.setClock(clockFrequency);  // call .setClock after .begin
}

void I2C_request(int16_t address, int16_t count, int16_t stop)
{
  Wire.requestFrom( address, count, stop);
}

uint16_t I2C_available()
{
  return Wire.available();
}

uint8_t I2C_read()
{
  return Wire.read();
}

void I2C_write(uint8_t data)
{
  Wire.write(data);
}

void I2C_beginTransmission(uint8_t address)
{
  Wire.beginTransmission(address);
}

void I2C_endTransmission(bool stop)
{
  Wire.endTransmission(stop);
}
