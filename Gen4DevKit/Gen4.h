#ifndef GEN4_H
#define GEN4_H

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS         0x00
#define BAD_CHECKSUM    0x01
#define LENGTH_MISMATCH 0x02

// Report IDs
#define MOUSE_ID    0x06
#define KEYBOARD_ID 0x08
#define ABSOLUTE_ID 0x09

// Common packet array elements
#define LENGTH_LOWBYTE    0
#define LENGTH_HIGHBYTE   1
#define REPORT_ID         2

// Mouse packet array elements
#define MOUSE_BUTTONS     3
#define X_DELTA           4
#define Y_DELTA           5
#define SCROLL_DELTA      6
#define PAN_DELTA         7

// Keyboard packet array elements
#define MODIFIER_KEYS     3
#define RESERVED          4
#define KEYCODE_1         5
#define KEYCODE_2         6
#define KEYCODE_3         7
#define KEYCODE_4         8
#define KEYCODE_5         9
#define KEYCODE_6         10

// Absolute packet array elements
#define CONTACTS_MASK     3
#define FINGER_0_PALM     4
#define FINGER_0_XLOW     5
#define FINGER_0_XHIGH    6
#define FINGER_0_YLOW     7
#define FINGER_0_YHIGH    8

#define FINGER_1_PALM     9
#define FINGER_1_XLOW     10
#define FINGER_1_XHIGH    11
#define FINGER_1_YLOW     12
#define FINGER_1_YHIGH    13

#define FINGER_2_PALM     14
#define FINGER_2_XLOW     15
#define FINGER_2_XHIGH    16
#define FINGER_2_YLOW     17
#define FINGER_2_YHIGH    18

#define FINGER_3_PALM     19
#define FINGER_3_XLOW     20
#define FINGER_3_XHIGH    21
#define FINGER_3_YLOW     22
#define FINGER_3_YHIGH    23

#define FINGER_4_PALM     24
#define FINGER_4_XLOW     25
#define FINGER_4_XHIGH    26
#define FINGER_4_YLOW     27
#define FINGER_4_YHIGH    28

#define ABSOLUTE_BUTTONS  29

/*  Public Gen4 Functions */

// Primary Gen4 Functions
void Gen4_init(void);
volatile bool Gen4_available(void);
void Gen4_readPacket(uint8_t * data);
void Gen4_dataReadyISR(void);
uint8_t Gen4_readExtendedMemory(uint32_t, uint8_t *, uint16_t);
void Gen4_writeExtendedMemory(uint32_t, uint8_t *, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
