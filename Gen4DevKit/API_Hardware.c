// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include "API_Hardware.h"
#include <Arduino.h>

void API_Hardware_delay(uint32_t ms)
{
    delay(ms);  //wraps the internal Arduino hardware delay
}