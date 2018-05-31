// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <stdint.h>
#include <Wire.h>
#include "I2C.h"

void I2C_init(uint32_t clockFrequency)
{
  Wire.begin();
  Wire.setClock(clockFrequency);  // call .setClock after .begin
}

void I2C_request(uint16_t address, uint16_t count, uint16_t stop)
{
  Wire.requestFrom(address, count, stop);
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