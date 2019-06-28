#ifndef I2C_H
#define I2C_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/** Required I2C API - The touch system requires the following I2C functionality from
the host:
*/
/** Set the Arduino as a master if no address is given and sets clock frequency */
void I2C_init(uint32_t clockFrequency);

/*
 * request the number of bytes specified by "count" from the given slave address
 * specified by "address". After transfer of data, set boolean "stop" as true to 
 * release the line. false will keep the line busy to send a restart.
 */
void I2C_request(int16_t address, int16_t count, bool stop);

/*
 * Returns the number of bytes available for reading.
 */
uint16_t I2C_available(void);

/*
 * returns the next byte. (Reads a byte that was transmitted from slave device to a master)
 */
uint8_t I2C_read(void);

/*
 * returns the number of bytes written. Writes data from a slave device from a request form a master.
 */
void I2C_write(uint8_t data);

/*
 * Begins the transmission to a I2C slave device with the given address. 
 */
void I2C_beginTransmission(uint8_t address);

/*
 * Ends the transmission to a slave deivce that was begun by the begin transmission. 
 * Boolean "stop" if true, sends a stop condiction, releasing the bus. If false, sendsa
 * restart request, keeping the connection active.
 */
void I2C_endTransmission(bool stop);

#ifdef __cplusplus
}
#endif

#endif
