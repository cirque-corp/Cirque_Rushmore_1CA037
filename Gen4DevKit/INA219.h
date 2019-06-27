#ifndef INA219_H
#define INA219_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#ifdef __cplusplus
extern "C" {
#endif
#include <Arduino.h>

// Congig Register Masks
#define CONFIG__FS_RANGE_16V      0x0000
#define CONFIG__FS_RANGE_32V      0x2000

#define CONFIG__SHUNT_PGA_DIV1    0x0000
#define CONFIG__SHUNT_PGA_DIV2    0x0800
#define CONFIG__SHUNT_PGA_DIV4    0x1000
#define CONFIG__SHUNT_PGA_DIV8    0x1800

#define CONFIG__SHUNT_ADC_AVERAGE_1   (0x0008 << 3) // 0.586ms max
#define CONFIG__SHUNT_ADC_AVERAGE_2   (0x0009 << 3) // 1.1ms
#define CONFIG__SHUNT_ADC_AVERAGE_4   (0x000A << 3) // 2.1ms
#define CONFIG__SHUNT_ADC_AVERAGE_8   (0x000B << 3) // 4.3ms
#define CONFIG__SHUNT_ADC_AVERAGE_16  (0x000C << 3) // 8.5ms
#define CONFIG__SHUNT_ADC_AVERAGE_32  (0x000D << 3) // 17ms
#define CONFIG__SHUNT_ADC_AVERAGE_64  (0x000E << 3) // 34ms
#define CONFIG__SHUNT_ADC_AVERAGE_128 (0x000F << 3) // 68ms
#define CONFIG__BUS_ADC_AVERAGE_1     (0x0008 << 7) // 0.5ms
#define CONFIG__BUS_ADC_AVERAGE_2     (0x0009 << 7) // 1.1ms
#define CONFIG__BUS_ADC_AVERAGE_4     (0x000A << 7) // 2.1ms
#define CONFIG__BUS_ADC_AVERAGE_8     (0x000B << 7) // 4.3ms
#define CONFIG__BUS_ADC_AVERAGE_16    (0x000C << 7) // 8.5ms
#define CONFIG__BUS_ADC_AVERAGE_32    (0x000D << 7) // 17ms
#define CONFIG__BUS_ADC_AVERAGE_64    (0x000E << 7) // 34ms
#define CONFIG__BUS_ADC_AVERAGE_128   (0x000F << 7) // 68ms

#define CONFIG_DELAY_1 (592)
#define CONFIG_DELAY_2 (2 * 592)
#define CONFIG_DELAY_4 (4 * 592)
#define CONFIG_DELAY_8 (8 * 592)
#define CONFIG_DELAY_16 (0)
#define CONFIG_DELAY_32 (0)
#define CONFIG_DELAY_64 (0)
#define CONFIG_DELAY_128 (0)
#define CONFIG_DELAY_POLL (0)

#define CONFIG__MODE_POWER_DOWN     0x0000
#define CONFIG__MODE_SHUNT_TRIG     0x0001
#define CONFIG__MODE_BUS_TRIG       0x0002
#define CONFIG__MODE_SHUNT_BUS_TRIG 0x0003
#define CONFIG__MODE_ADC_OFF        0x0004
#define CONFIG__MODE_SHUNT_CONT     0x0005
#define CONFIG__MODE_BUS_CONT       0x0006
#define CONFIG__MODE_SHUNT_BUS_CONT 0x0007

void INA219_reset(void);
void INA219_init(uint8_t slaveAddress);
void INA219_config(uint16_t data);
int32_t INA219_measureShuntVoltage(uint16_t averagingMask);
int32_t INA219_measureBusVoltage(uint16_t averagingMask, uint32_t delay);

void INA219_powerDown(void);

#ifdef __cplusplus
}
#endif

#endif
