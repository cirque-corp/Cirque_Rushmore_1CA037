// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include "Demo_02_000658.h"
#include <Arduino.h>
#include "Project_Config.h"

/// This code controls the extra hardware functions provided by the 02-000658 Glidepoint Dev Kit board (Blue Soldermask version)
/// The relay control board is assumed to be the 02-000663 board

// Demo board hardware definition
// Power supply control
#define V_SEL_PIN  (1)
// Latching relay - Touch injection control
#define RELAY1_PIN_P   (16)  // polarity set for the 02-000663 board
#define RELAY1_PIN_N   (17)
#define RELAY2_PIN_P   (22)  // polarity set for the 02-000663 board
#define RELAY2_PIN_N   (23)
// Button injection control
#define BTN1_PIN      (3)
#define BTN2_PIN      (4)
#define BTN3_PIN      (5)
// Haptic control
#define HAPTIC_PIN    (6)
// "Scope Trigger lines"
#define SCOPE1_PIN (7)
#define SCOPE2_PIN (8)

const uint8_t LED_Pins[] = { LED1_PIN, LED2_PIN };
const uint8_t NumberOfLeds = sizeof(LED_Pins) / sizeof(LED_Pins[0]);

void Demo_init(void)
{
  pinMode(V_SEL_PIN, OUTPUT);
  pinMode(RELAY1_PIN_P, OUTPUT);
  pinMode(RELAY1_PIN_N, OUTPUT);
  pinMode(RELAY2_PIN_P, OUTPUT);
  pinMode(RELAY2_PIN_N, OUTPUT);
  pinMode(SCOPE1_PIN, OUTPUT);
  pinMode(SCOPE2_PIN, OUTPUT);
  
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  pinMode(BTN3_PIN, INPUT);
  
  byte x;
  for (x = 0; x < NumberOfLeds; x++)
  {
	digitalWrite(LED_Pins[x], HIGH); // LED off
    pinMode(LED_Pins[x], OUTPUT);
  }  

  // Make Sure Relay is Open
  Demo_setRelay1(0);
  Demo_setRelay2(0);
  
  Demo_powerOff();
  
  INA219_init(0x40);
  INA219_config(CONFIG__FS_RANGE_16V | CONFIG__SHUNT_PGA_DIV8);
}

void Demo_setLED(uint8_t LEDNumber, uint8_t State)
{
	if (LEDNumber >= NumberOfLeds) return;
 
	digitalWrite(LED_Pins[LEDNumber], (State == 0) ? HIGH : LOW);
}

void Demo_powerOff(void)
{
  digitalWrite(V_SEL_PIN, LOW);
}

void Demo_unpowerBus(void)
{
  digitalWrite(CONFIG_HOST_DR_PIN, 0);
  pinMode(CONFIG_HOST_DR_PIN, OUTPUT);
  Demo_buttonDown(1);
  Demo_buttonDown(2);
  Demo_buttonDown(3);
  delayMicroseconds(1);
  pinMode(CONFIG_HOST_DR_PIN, INPUT);
  Demo_buttonUp(1);
  Demo_buttonUp(2);
  Demo_buttonUp(3);
}

void Demo_power3V3(void)
{
  Demo_powerOn();
}

void Demo_power5V0(void)
{
  Demo_powerOn();
}

void Demo_powerOn(void)
{
    digitalWrite(V_SEL_PIN, HIGH);
}

void Demo_setRelay1(uint8_t state)
{
  if (state)
  {
    digitalWrite(RELAY1_PIN_P, HIGH);
    digitalWrite(RELAY1_PIN_N, LOW);
  }
  else
  {
    digitalWrite(RELAY1_PIN_P, LOW);
    digitalWrite(RELAY1_PIN_N, HIGH);
  }
  delay(8);  
  digitalWrite(RELAY1_PIN_P, LOW);
  digitalWrite(RELAY1_PIN_N, LOW);
}

void Demo_setRelay2(uint8_t state)
{
  if (state)
  {
    digitalWrite(RELAY2_PIN_P, HIGH);
    digitalWrite(RELAY2_PIN_N, LOW);
  }
  else
  {
    digitalWrite(RELAY2_PIN_P, LOW);
    digitalWrite(RELAY2_PIN_N, HIGH);
  }
  delay(8);  
  digitalWrite(RELAY2_PIN_P, LOW);
  digitalWrite(RELAY2_PIN_N, LOW);
}

void Demo_buttonDown(uint8_t num)
{
  if (num == 1)
  {
    digitalWrite(BTN1_PIN, LOW);
    pinMode(BTN1_PIN, OUTPUT);
  }
  else if (num == 2)
  {
    digitalWrite(BTN2_PIN, LOW);
    pinMode(BTN2_PIN, OUTPUT);
  }
  else if (num == 3)
  {
    digitalWrite(BTN3_PIN, LOW);
    pinMode(BTN3_PIN, OUTPUT);
  }
}

void Demo_buttonUp(uint8_t num)
{
  if (num == 1)
  {
    pinMode(BTN1_PIN, INPUT);
  }
  else if (num == 2)
  {
    pinMode(BTN2_PIN, INPUT);
  }
  else if (num == 3)
  {
    pinMode(BTN3_PIN, INPUT);
  }
}

void Demo_pressButton(uint8_t num, int Duration_usec)
{
  Demo_buttonDown(num);
  delayMicroseconds(Duration_usec);
  Demo_buttonUp(num);
}

uint8_t Demo_buttonIsDown(uint8_t num)
{ 
  if (num == 1)
  {
    return digitalRead(BTN1_PIN) == LOW;
  }
  else if (num == 2)
  {
    return digitalRead(BTN2_PIN) == LOW;
  }
  else if (num == 3)
  {
    return digitalRead(BTN3_PIN) == LOW;
  }
  return false;
}

void Demo_scopeTrigger1(void)
{
	digitalWrite(SCOPE1_PIN, HIGH);
	delayMicroseconds(1);
	digitalWrite(SCOPE1_PIN, LOW);
}

void Demo_scopeTrigger2(void)
{
	digitalWrite(SCOPE2_PIN, HIGH);
	delayMicroseconds(1);
	digitalWrite(SCOPE2_PIN, LOW);
}
