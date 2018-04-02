#ifndef I2C_H
#define I2C_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license
// I2C public API - users will need to implement these functions for their MCU:

#ifdef __cplusplus
extern "C" {
#endif

void I2C_init(uint32_t);
void I2C_request(uint16_t, uint16_t, uint16_t);
uint16_t I2C_available(void);
uint8_t I2C_read(void);
void I2C_write(uint8_t);
void I2C_beginTransmission(uint8_t);
void I2C_endTransmission(bool);

#ifdef __cplusplus
}
#endif

#endif
