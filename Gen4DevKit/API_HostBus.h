#ifndef API_HOSTBUS_H
#define API_HOSTBUS_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS         0x00
#define BAD_CHECKSUM    0x01
#define LENGTH_MISMATCH 0x02
#define CIRQUE_SLAVE_ADDR  0x2A
#define ALPS_SLAVE_ADDR  0x2C

/**
@defgroup host_interface The I2C Host Interface
The I2C Host Interface to a Cirque Touch System consists of three parts:
1) Power supply
2) I2C interface (SCL, SDA)
3) Host Data Ready line (Host_DR)
This section defines the operation of those three parts.
*/
/*  Public  Functions */

/** The I2C bus commonly runs at a clock frequency of 400kHz.
The operation of touch system is also tested at 100kHz. 
*/
void HB_init(int I2CFrequency, uint8_t I2CAddress);

/** The Host DR Line (Host_DR) is a an output from the touch system that signals when
there is data to be read. The line two states: Asserted (data is available), and de-asserted
(no data is available). The details of the operation of that line are shown in
HB_DR_Asserted().
*/
bool HB_DR_Asserted(void);

/** The most common I2C action is a "report read" operation to transfer touch information from the touch system
to the host. The details of the read operation are shown in HB_readReport().
*/
void HB_readReport(uint8_t * packet, uint16_t readLength);

/** The touch system functionality is controlled using the Entended Memory Access operations. The details
of the memory access process are shown in HB_readExtendedMemory() and HB_writeExtendedMemory();
*/
uint8_t HB_readExtendedMemory(uint32_t, uint8_t *, uint16_t);
void HB_writeExtendedMemory(uint32_t, uint8_t *, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
