#ifndef API_HARDWARE_H
#define API_HARDWARE_H

#include <stdint.h>

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

// These functions must be implemented for the Gen4 API to work
#ifdef __cplusplus
extern "C" {
#endif

/** Initialize Hardware information */
void API_Hardware_init(void);

/** Power Up the Hardware */
void API_Hardware_PowerOn(void);

/** Power off the Hardware */
void API_Hardware_PowerOff(void);

/** Delay for ms milliseconds.
*/
void API_Hardware_delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // API_HARDWARE_H

