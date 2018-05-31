// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include <Arduino.h>
#include "UsbSerial.h"

void UsbSerial_Init()
{
  Serial.begin(115200);
  delay(1000);
//  while(!Serial);
}

void UsbSerial_SendShort(uint16_t shortToSend)
{
  Serial.write((uint8_t)(shortToSend));
  Serial.write((uint8_t)(shortToSend >> 8));
}

void UsbSerial_SendByteBuffer(uint8_t * pBuffer, uint32_t numBytes)
{
  for(uint32_t i = 0; i < numBytes; i++)
  {
    Serial.write(pBuffer[i]);
  }
}
