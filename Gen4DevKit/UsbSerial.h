#ifndef __USB_SERIAL_H__
#define __USB_SERIAL_H__

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#ifdef __cplusplus
extern "C" {
#endif

void UsbSerial_Init(void);
void UsbSerial_SendShort(uint16_t);
void UsbSerial_SendByteBuffer(uint8_t *, uint32_t);

#ifdef __cplusplus
}
#endif

#endif  //__USB_SERIAL_H__
