// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <stdint.h>
#include <stdbool.h>
#include "I2C.h"
#include "HostDR.h"
#include "API_HostBus.h"


uint8_t deviceAddress;

void HB_init(int I2CFrequency, uint8_t I2CAddress)
{
  I2C_init(I2CFrequency);
  deviceAddress = I2CAddress;
  HostDR_init();
}

bool HB_DR_Asserted(void)
{
  return (HostDR_pinState() == 0);
}

void HB_readReport(uint8_t * reportData, uint16_t readLength)
{
  uint8_t i = 0;
 
  I2C_request((uint16_t)deviceAddress, readLength, (uint16_t)true);

  while(I2C_available())
  {
    reportData[i++] = I2C_read();
  }
}

void HB_writeExtendedMemory(uint32_t registerAddress, uint8_t * data, uint8_t count)
{
  uint8_t checksum = 0, i = 0;
  uint8_t preamble[8] = 
  {
    0x00,
    0x09,
    (uint8_t)(registerAddress & 0x000000FF),
    (uint8_t)((registerAddress & 0x0000FF00)>>8),
    (uint8_t)((registerAddress & 0x00FF0000)>>16),
    (uint8_t)((registerAddress & 0xFF000000)>>24),
    (uint8_t)(count & 0x00FF),
    (uint8_t)((count & 0xFF00) >> 8)
  };

  I2C_beginTransmission(deviceAddress);
  for(; i < 8; i++)
  {
    I2C_write(preamble[i]);
    checksum += preamble[i];
  }    

  for(i = 0; i < count; i++)
  {
    I2C_write(data[i]);
    checksum += data[i];
  }
  I2C_write(checksum);
  I2C_endTransmission(true);
}

uint8_t HB_readExtendedMemory(uint32_t registerAddress, uint8_t * data, uint16_t count)
{
  uint8_t checksum = 0, result = SUCCESS;
  uint16_t i = 0, bytesRead = 0;
  uint8_t lengthBytes[2];
  uint8_t preamble[8] = 
  {
    0x01,
    0x09,
    (uint8_t)(registerAddress & (uint32_t)0x000000FF),
    (uint8_t)((registerAddress & 0x0000FF00)>>8),
    (uint8_t)((registerAddress & 0x00FF0000)>>16),
    (uint8_t)((registerAddress & 0xFF000000)>>24),
    (uint8_t)(count & 0x00FF),
    (uint8_t)((count & 0xFF00) >> 8)
  };
  
  // Send extended memory access command to Gen4
  I2C_beginTransmission(deviceAddress);
  for(; i < 8; i++)
  {
    I2C_write(preamble[i]);
  }    
  I2C_endTransmission(false);
  
  // Read requested data from Gen4, plus overhead (3 extra bytes for lengthLow, lengthHigh, & checksum)
  I2C_request(deviceAddress, count + 3, true);

  // Read first 2 bytes (lower and upper length-bytes)
  for(i = 0; i < 2; i++)
  {
    checksum += lengthBytes[i] = I2C_read();
    bytesRead++;
  }

  // Read data bytes requested by caller
  for(i = 0; i < count; i++)
  {
    checksum += data[i] = I2C_read();
    bytesRead++;
  }

  // Read the and check the last byte (the checksum byte)
  if(checksum != I2C_read())
  {
    result |= BAD_CHECKSUM;
  }

  if(++bytesRead != (lengthBytes[0] | (lengthBytes[1] << 8)))
  {
    result |= LENGTH_MISMATCH;
  }

  return result;
}


