// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license


#include "API_HostBus.h"

uint8_t _deviceAddress;

/************************************************************/
/************************************************************/
/********************  PUBLIC FUNCTIONS *********************/

/** The I2C bus commonly runs at a clock frequency of 400kHz.
	The operation of touch system is also tested at 100kHz. */
void HB_init(int I2CFrequency, uint8_t I2CAddress)
{
  I2C_init(I2CFrequency);
  _deviceAddress = I2CAddress;
  HostDR_init();
}

/** The Host DR Line (Host_DR) is a an output from the touch system that signals when
	there is data to be read. The line two states: Asserted (data is available), 
	and de-asserted (no data is available). The details of the operation of 
	that line are shown in HB_DR_Asserted(). */
bool HB_DR_Asserted(void)
{
  return (HostDR_pinState() == 0);
}

/** The most common I2C action is a "report read" operation to transfer touch 
	information from the touch system to the host. The details of the read 
	operation are shown in HB_readReport(). */
void HB_readReport(uint8_t * reportData, uint16_t readLength)
{
  uint8_t i = 0;
 
  I2C_request((uint16_t)_deviceAddress, readLength, (uint16_t)true);

  while(I2C_available())
  {
    reportData[i++] = I2C_read();
  }
}

/** The touch system functionality is controlled using the Entended Memory 
	Access operations. The details of the memory access process are shown 
	in HB_readExtendedMemory() and HB_writeExtendedMemory(); */
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
  I2C_beginTransmission(_deviceAddress);
  for(; i < 8; i++)
  {
    I2C_write(preamble[i]);
  }    
  I2C_endTransmission(false);
  
  /* Read requested data from Gen4, plus overhead 
	(3 extra bytes for lengthLow, lengthHigh, & checksum)
  */
  I2C_request(_deviceAddress, count + 3, true);

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

  I2C_beginTransmission(_deviceAddress);
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
