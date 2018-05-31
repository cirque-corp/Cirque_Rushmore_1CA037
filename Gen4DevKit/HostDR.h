#ifndef HostDR_H
#define HostDR_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

/** Required Host_DR API - The touch system requires the following Host_DR
functionality:
*/
/** Initialize the Host_DR line as an input.  No pull up is required as the line
is driven high and low by the touch system
*/
void HostDR_init(void);
/** Read the Host_DR line's state; either 0 or 1.
*/
bool HostDR_pinState(void);


#ifdef __cplusplus
}
#endif

#endif