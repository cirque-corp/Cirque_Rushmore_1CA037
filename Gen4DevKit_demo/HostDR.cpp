// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include "HostDR.h"
#include "Arduino.h"
#include "Project_Config.h"

void HostDR_init(void)
{
	pinMode(CONFIG_HOST_DR_PIN, INPUT);
}

bool HostDR_pinState(void)
{
	return digitalRead(CONFIG_HOST_DR_PIN);
}


