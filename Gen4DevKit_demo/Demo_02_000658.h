#ifndef DEMO_02_000658_H
#define DEMO_02_000658_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Demo board (Blue Soldermask version) also needs INA219.h 
#include <stdint.h>
#include "INA219.h"

#define SHUNT_RESISTOR  (1.0)
#define SHUNT_OFFSET    (-0.1166)  	// Found by comparing measurements to calibrated meter
#define SHUNT_SLOPE     (0.9988)    // Found by comparing measurements to calibrated meter

#define LED1_PIN      (20)
#define LED2_PIN      (21)

void Demo_init(void);
void Demo_setLED(uint8_t LEDNumber, uint8_t State);
void Demo_powerOff(void);
void Demo_unpowerBus(void);
void Demo_power3V3(void);       //depreciated
void Demo_power5V0(void);       //depreciated
void Demo_powerOn(void);
void Demo_setRelay1(uint8_t state);
void Demo_setRelay2(uint8_t state);
void Demo_buttonDown(uint8_t num);
void Demo_buttonUp(uint8_t num);
void Demo_pressButton(uint8_t num, int Duration_usec);
uint8_t Demo_buttonIsDown(uint8_t num);
void Demo_scopeTrigger1(void);
void Demo_scopeTrigger2(void);

#ifdef __cplusplus
}
#endif

#endif
