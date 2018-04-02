// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <stdint.h>
#include <stdbool.h>
#include "I2C.h"
#include "Gen4.h"

#define SLAVE_ADDR  0x2A

// Globals
volatile bool _dataReady = false;

/*  Public Gen4 Functions */
void Gen4_init()
{
  I2C_init(400000);
}

void Gen4_dataReadyISR()
{
  _dataReady = true;
}

volatile bool Gen4_available()
{
  return _dataReady;
}

void Gen4_readPacket(uint8_t * data)
{
  uint8_t i = 0;
  _dataReady = false;
  
  I2C_request((uint16_t)SLAVE_ADDR, (uint16_t)53, (uint16_t)true);  // Read first 3 bytes to get to report ID

  while(I2C_available())
  {
    data[i++] = I2C_read();
  }
}

void Gen4_writeExtendedMemory(uint32_t registerAddress, uint8_t * data, uint8_t count)
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

  I2C_beginTransmission(SLAVE_ADDR);
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

uint8_t Gen4_readExtendedMemory(uint32_t registerAddress, uint8_t * data, uint16_t count)
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
  I2C_beginTransmission(SLAVE_ADDR);
  for(; i < 8; i++)
  {
    I2C_write(preamble[i]);
  }    
  I2C_endTransmission(false);
  
  // Read requested data from Gen4, plus overhead (3 extra bytes for lengthLow, lengthHigh, & checksum)
  I2C_request(SLAVE_ADDR, count + 3, true);

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
    
    // TODO: not sure if bytesRead should be incremented here or not
    bytesRead++;
  }

  if(bytesRead != (lengthBytes[0] | (lengthBytes[1] << 8)))
  {
    result |= LENGTH_MISMATCH;
  }

  return result;
}
