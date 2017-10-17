// I2C public API - users will need to implement these functions for their MCU:
#ifndef I2C_H
#define I2C_H

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
