// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <stdint.h>
#include "INA219.h"
#include "I2C.h"

#define REGISTER__CONFIG            0x00
#define REGISTER__SHUNT_VOLTAGE     0x01
#define REGISTER__BUS_VOLTAGE       0x02
#define REGISTER__POWER             0x03
#define REGISTER__CURRENT           0x04
#define REGISTER__CALIBRATION       0x05

#define CONFIG__RESET               0x8000
#define CONFIG__BUS_16V             0x0000
#define CONFIG__BUS_32V             0x2000

#define CONFIG__BUS_ADC_RES_9       0x0000
#define CONFIG__BUS_ADC_RES_10      0x0080
#define CONFIG__BUS_ADC_RES_11      0x0100
#define CONFIG__BUS_ADC_RES_12      0x0180
#define CONFIG__BUS_ADC_AVERAGING   0x0400

#define CONFIG__SHUNT_ADC_RES_9     0x0000
#define CONFIG__SHUNT_ADC_RES_10    0x0008
#define CONFIG__SHUNT_ADC_RES_11    0x0010
#define CONFIG__SHUNT_ADC_RES_12    0x0018
#define CONFIG__SHUNT_ADC_AVERAGING 0x0040

static uint8_t _slaveAddress = 0x40;

/*  Private Functions */

static void WriteRegister(uint8_t reg, uint16_t value)
{
  I2C_beginTransmission(_slaveAddress);
  I2C_write(reg);
  I2C_write((value >> 8) & 0xFF);
  I2C_write(value & 0xFF);
  I2C_endTransmission(true);
}

static uint16_t ReadRegister(uint8_t reg)
{
  I2C_beginTransmission(_slaveAddress);
  I2C_write(reg);
  I2C_endTransmission(false);  // NOTE: according to 8.5.6, no STOP condition
  I2C_request(_slaveAddress, 2, true);
  return ((I2C_read() << 8) | I2C_read());
}

// Triggers a conversion for current measurement. The number of samples to be averaged (internal to INA219)
// is set by INA219_shuntADCAveraging()
static void INA219_triggerShuntMeasurement(uint16_t averagingMask)
{
  uint16_t temp = ReadRegister(REGISTER__CONFIG);
  temp &= ~CONFIG__SHUNT_ADC_AVERAGE_128;
  temp |= CONFIG__MODE_SHUNT_TRIG | averagingMask;
  WriteRegister(REGISTER__CONFIG, temp);
}

// Triggers a conversion for current measurement. The number of samples to be averaged (internal to INA219)
// is set by INA219_shuntADCAveraging()
static void INA219_triggerBusMeasurement(uint16_t averagingMask)
{
  uint16_t temp = ReadRegister(REGISTER__CONFIG);
  temp &= ~CONFIG__SHUNT_ADC_AVERAGE_128;
  temp |= CONFIG__MODE_BUS_TRIG | averagingMask;
  WriteRegister(REGISTER__CONFIG, temp);
}

/*  Public Functions  */

// Assigns an I2C slave address and opens the I2C port @ 400kHz
// NOTE: This function must be called before calling any other INA219 functions
void INA219_init(uint8_t slaveAddress)
{
  _slaveAddress = slaveAddress;
  I2C_init(400000);
  
  WriteRegister(REGISTER__CALIBRATION, 13421);
}

// Writes <data> to the config register (Reg: 0x00)
// NOTE: Use CONFIG__ register masks in INA219.h for easy configuration
void INA219_config(uint16_t data)
{
  WriteRegister(REGISTER__CONFIG, data);
}

// Resets all internal INA219 registers to POR defaults
void INA219_reset(void)
{
  uint16_t temp = ReadRegister(REGISTER__CONFIG);
  temp |= CONFIG__RESET;
  WriteRegister(REGISTER__CONFIG, temp);
}

// Checks the Conversion Ready bit (CNVR) in INA219, returns true if set
// INA219 sets this bit when conversions, plus any additional calculations are complete
bool INA219_dataReady(void)  // Status Register, CNVR bit
{
  if((ReadRegister(REGISTER__BUS_VOLTAGE) & 0x0002) != 0) return true;
  
  return false;
}

// Measures voltage between IN+ and IN- (across shunt resistor) in uV
// NOTE: Use CONFIG__SHUNT_ADC_AVERAGE masks in INA219.h for setting
int32_t INA219_measureShuntVoltage(uint16_t averagingMask)
{
  int32_t temp = 0;
  uint32_t delay = 0;
  
  INA219_triggerShuntMeasurement(averagingMask);
  
  switch(averagingMask)
  {
	case CONFIG__SHUNT_ADC_AVERAGE_1:
		delay = CONFIG_DELAY_1;
		break;	
	case CONFIG__SHUNT_ADC_AVERAGE_2:
		delay = CONFIG_DELAY_2;
		break;
	case CONFIG__SHUNT_ADC_AVERAGE_4:
		delay = CONFIG_DELAY_4;
		break;
	case CONFIG__SHUNT_ADC_AVERAGE_8:
		delay = CONFIG_DELAY_8;
		break;	
	case CONFIG__SHUNT_ADC_AVERAGE_16:
		delay = CONFIG_DELAY_16;
		break;
	case CONFIG__SHUNT_ADC_AVERAGE_32:
		delay = CONFIG_DELAY_32;
		break;
	case CONFIG__SHUNT_ADC_AVERAGE_64:
		delay = CONFIG_DELAY_64;
		break;
	case CONFIG__SHUNT_ADC_AVERAGE_128:
		delay = CONFIG_DELAY_128;
		break;
  }
  
  if (delay > 0)
  {
	  delayMicroseconds(delay);
  }
  else
  {
	  while(!INA219_dataReady());
  }
  temp = (int16_t) ReadRegister(REGISTER__SHUNT_VOLTAGE);
  temp *= 10;
  return temp;
}

// Measures voltage at IN- pin in mV
// NOTE: Use CONFIG__BUS_ADC_AVERAGE masks in INA219.h for setting
int32_t INA219_measureBusVoltage(uint16_t averagingMask, uint32_t delay)
{
  INA219_triggerBusMeasurement(averagingMask);
  if (delay > 0)
  {
	  delayMicroseconds(delay);
  }
  else
  {
	  while(!INA219_dataReady());
  }
  return (int32_t)((ReadRegister(REGISTER__BUS_VOLTAGE) >> 3) * 4);
}
