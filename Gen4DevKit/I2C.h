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
/** The ability to initialize and configure the I2C system for a given clock frequency */
void I2C_init(uint32_t clockFrequency);
/** finish these comments */
void I2C_request(int16_t address, int16_t count, int16_t stop);
uint16_t I2C_available(void);
uint8_t I2C_read(void);
void I2C_write(uint8_t data);
void I2C_beginTransmission(uint8_t);
void I2C_endTransmission(bool stop);

#ifdef __cplusplus
}
#endif

#endif
