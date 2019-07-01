#ifndef API_HOSTBUS_H
#define API_HOSTBUS_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license
/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "I2C.h"
#include "HostDR.h"

#define SUCCESS           0x00
#define BAD_CHECKSUM      0x01
#define LENGTH_MISMATCH   0x02
#define CIRQUE_SLAVE_ADDR 0x2A
#define ALPS_SLAVE_ADDR   0x2C

/************************************************************/
/************************************************************/
/********************  PUBLIC FUNCTIONS *********************/

/** @defgroup host_interface The I2C Host Interface
	The I2C Host Interface to a Cirque Touch System consists of three parts:
	1) Power supply
	2) I2C interface (SCL, SDA)
	3) Host Data Ready line (Host_DR)
	This section defines the operation of those three parts. */

void HB_init(int I2CFrequency, uint8_t I2CAddress);

bool HB_DR_Asserted(void);

void HB_readReport(uint8_t * packet, uint16_t readLength);

uint8_t HB_readExtendedMemory(uint32_t, uint8_t *, uint16_t);

void HB_writeExtendedMemory(uint32_t, uint8_t *, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
