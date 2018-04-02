// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include "Gen4.h"
#include "INA219.h"

//  Hardware Interface
//  GND
//  +3.3V
//  SDA = Pin 18  (there MUST be a pull-up to 3.3V on this signal; 4.7k recommended)
//  SCL = Pin 19  (there MUST be a pull-up to 3.3V on this signal; 4.7k recommended)
//  DR = Pin 9

#define DR_PIN        (9)
#define V3P3_SEL_PIN  (1)
#define V5P0_SEL_PIN  (2)
#define RELAY_PIN_P   (23)
#define RELAY_PIN_N   (22)
#define BTN1_PIN      (3)
#define BTN2_PIN      (4)
#define BTN3_PIN      (5)

#define SHUNT_RESISTOR  (1.0)
#define SHUNT_OFFSET    (-0.1166)  // Found by comparing measurements to calibrated meter
#define SHUNT_SLOPE     (0.9988)    // Found by comparing measurements to calibrated meter

uint8_t gen4Buffer[53];

bool monitorCurrent = false;

// setup() gets called once at power-up
void setup()
{  
  Serial.begin(115200);
  while(!Serial);
//  delay(750);

  pinMode(V3P3_SEL_PIN, OUTPUT);
  pinMode(V5P0_SEL_PIN, OUTPUT);
  pinMode(RELAY_PIN_P, OUTPUT);
  pinMode(RELAY_PIN_N, OUTPUT);
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  pinMode(BTN3_PIN, INPUT);

// Make Sure Relay is Open
  SetRelay(0);
  
  PowerOff();
  delay(100);
  Power3V3();
  delay(100);
  
  INA219_init(0x40);
  INA219_config(CONFIG__FS_RANGE_16V | CONFIG__SHUNT_PGA_DIV8);
  
  Serial.println("Running Gen4 I2C Demo...");
  
  Gen4_init();
  Demo_setZIdleCount(5);
  
  pinMode(DR_PIN, INPUT);
  attachInterrupt(DR_PIN, dataReadyISR, FALLING);

  Serial.print("Chip ID:\t0x");
  Serial.println(Demo_getChipID(), HEX);
  Serial.print("FW Version:\t0x");
  Serial.println(Demo_getFirmwareVersion(), HEX);
  Serial.print("FW Sub-Version:\t0x");
  Serial.println(Demo_getFirmwareSubversion(), HEX);
}

void loop()
{  
  if(monitorCurrent)
  {
    Serial.print("Current:\t");
    Serial.println(SHUNT_SLOPE * ((float)INA219_measureShuntVoltage(CONFIG__SHUNT_ADC_AVERAGE_128) / 1000.0) + SHUNT_OFFSET, 3);
  }
  if(Gen4_available())
  {
    Gen4_readPacket(gen4Buffer);
    printPacket(gen4Buffer);
  }
  if(Serial.available())
  {
    char rxChar = Serial.read();
    switch(rxChar)
    {
      case 'b':
        PressButton(1);
        break;
      case 'c':
        Demo_forceComp();
        Serial.println("Module Compensated...");
        break;
      case 't':
        SetRelay(1);
        delay(1000);
        SetRelay(0);
        break;
      case 'e':
        Demo_enableFeed();
        Serial.println("Feed enabled...");
        break;
      case 'd':
        Demo_disableFeed();
        Serial.println("Feed disabled...");
        break;
      case 'a':
        Demo_setToAbsoluteMode();
        Serial.println("Switched to absolute mode...");
        break;
      case 'r':
        Demo_setToRelativeMode();
        Serial.println("Switched to relative mode...");
        break;       
      case 'p':
        Demo_persistExtendedMemory();
        Serial.println("Config saved to NV RAM...");
        break;
      case '0':
        Demo_disableLowPowerModes();
        Serial.println("Low power modes disabled...");
        break;
      case '1':
        Demo_enableStage1Sleep();
        Serial.println("Sleep (stage 1) enabled...");
        break;
      case '2':
        Demo_enableStage2Sleep();
        Serial.println("Sleep (stage 2) enabled...");
        break;
      case 'z':
        Demo_setZIdleCount(5);
        Serial.println("Z-idle count changed to 5...");
        break;
      case '3':
        Power3V3();
        Serial.println("V-OUT set to 3.3V...");
        break;
      case '5':
        Power5V0();
        Serial.println("V-OUT set to 5.0V...");
        break;
      case 'n':
        digitalWrite(V3P3_SEL_PIN, LOW);
        digitalWrite(V5P0_SEL_PIN, LOW);
        Serial.println("Output disabled...");
        break;
      case 'i':
      {
        if(monitorCurrent)
        {
          monitorCurrent = false;
        }
        else
        {
          monitorCurrent = true;
        }        
        break;
      }
      default:
        Serial.println("Invalid input...");
        break;
    }
  }
}

void PowerOff()
{
  digitalWrite(V5P0_SEL_PIN, LOW);
  digitalWrite(V3P3_SEL_PIN, LOW);
}

void Power3V3()
{
  digitalWrite(V5P0_SEL_PIN, LOW);
  digitalWrite(V3P3_SEL_PIN, HIGH);
}

void Power5V0()
{
  digitalWrite(V3P3_SEL_PIN, LOW);
  digitalWrite(V5P0_SEL_PIN, HIGH);
}

void SetRelay(bool state)
{
  if (state)
  {
    digitalWrite(RELAY_PIN_P, HIGH);
    digitalWrite(RELAY_PIN_N, LOW);
    delay(200);
    digitalWrite(RELAY_PIN_P, LOW);
    digitalWrite(RELAY_PIN_N, LOW);
  }
  else
  {
    digitalWrite(RELAY_PIN_P, LOW);
    digitalWrite(RELAY_PIN_N, HIGH);
    delay(200);
    digitalWrite(RELAY_PIN_P, LOW);
    digitalWrite(RELAY_PIN_N, LOW);
  }
}

void PressButton (uint8_t num)
{
  if (num == 1)
  {
    digitalWrite(BTN1_PIN, LOW);
    pinMode(BTN1_PIN, OUTPUT);
    delay (500);
    pinMode(BTN1_PIN, INPUT);
  }
  else if (num == 2)
  {
    digitalWrite(BTN2_PIN, LOW);
    pinMode(BTN2_PIN, OUTPUT);
    delay (500);
    pinMode(BTN2_PIN, INPUT);
  }
  else if (num == 3)
  {
    digitalWrite(BTN3_PIN, LOW);
    pinMode(BTN3_PIN, OUTPUT);
    delay (500);
    pinMode(BTN3_PIN, INPUT);
  }
}

// This ISR triggers on a falling edge of the HOST_DR signal (from Gen4)
// Designers must implement this ISR and call Gen4_dataReadyISR() inside it.
void dataReadyISR()
{
  Gen4_dataReadyISR();
}

/*  Functions that demonstrate how to use Gen4.h functions */
uint8_t Demo_getRegisterContents(uint32_t address)
{
  uint8_t contents = 0;
  Gen4_readExtendedMemory(address, &contents, 1);
  return contents;
}

void Demo_persistExtendedMemory()
{
  uint8_t persistentDataControl = Demo_getRegisterContents(0xC2DF);
  persistentDataControl |= 0x01;
  Gen4_writeExtendedMemory(0xC2DF, &persistentDataControl, 1);
}

uint8_t Demo_getChipID(void)
{
  return Demo_getRegisterContents(0xC2C0);
}

uint8_t Demo_getFirmwareVersion()
{
  return Demo_getRegisterContents(0xC2C1);
}

uint8_t Demo_getFirmwareSubversion()
{
  return Demo_getRegisterContents(0xC2DC);
}

void Demo_setToAbsoluteMode()
{
  uint8_t feedConfig1 = Demo_getRegisterContents(0xC2C4);
  feedConfig1 |= 0x02;
  Gen4_writeExtendedMemory(0xC2C4, &feedConfig1, 1);
}

void Demo_setToRelativeMode()
{
  uint8_t feedConfig1 = Demo_getRegisterContents(0xC2C4);
  feedConfig1 &= ~0x02;
  Gen4_writeExtendedMemory(0xC2C4, &feedConfig1, 1);  
}

void Demo_enableFeed()
{
  uint8_t feedConfig1 = Demo_getRegisterContents(0xC2C4);
  feedConfig1 |= 0x01;
  Gen4_writeExtendedMemory(0xC2C4, &feedConfig1, 1); 
}

void Demo_disableFeed()
{
  uint8_t feedConfig1 = Demo_getRegisterContents(0xC2C4);
  feedConfig1 &= ~0x01;
  Gen4_writeExtendedMemory(0xC2C4, &feedConfig1, 1);  
}

void Demo_setZIdleCount(uint8_t zIdleCount)
{
  Gen4_writeExtendedMemory(0xC2CA, &zIdleCount, 1);
}

void Demo_disableLowPowerModes()
{
  uint8_t powerControl = Demo_getRegisterContents(0xC2CE);
  powerControl &= ~0x01;
  powerControl |= 0x10;
  Gen4_writeExtendedMemory(0xC2CE, &powerControl, 1);
}

void Demo_enableStage1Sleep()
{
  uint8_t powerControl = Demo_getRegisterContents(0xC2CE);
  powerControl |= 0x01;
  Gen4_writeExtendedMemory(0xC2CE, &powerControl, 1);
}

void Demo_enableStage2Sleep()
{
  uint8_t powerControl = Demo_getRegisterContents(0xC2CE);
  powerControl &= ~0x10;
  Gen4_writeExtendedMemory(0xC2CE, &powerControl, 1);
}

void Demo_forceComp()
{
  uint8_t feedConfig1 = Demo_getRegisterContents(0xC2C4);
  feedConfig1 |= 0x80;
  Gen4_writeExtendedMemory(0xC2C4, &feedConfig1, 1);
}

// Functions for formatting/displaying data in serial terminal
void printPacket(uint8_t * data)
{
  switch(data[REPORT_ID])
  {
    case MOUSE_ID:
      printMouseData(data);
      break;
    case KEYBOARD_ID:
      printKeyboardData(data);
      break;
    case ABSOLUTE_ID:
      printAbsoluteData(data);
      break;
    default:
      Serial.println("ERROR: Unsupported report-ID...");
  }
}

void printMouseData(uint8_t * data)
{
  Serial.print("Length:\t");
  Serial.println(((uint16_t)data[LENGTH_HIGHBYTE]) << 8 | (uint16_t)data[LENGTH_LOWBYTE]);
  Serial.print("Report ID:\t");
  Serial.println(data[REPORT_ID], HEX);    
  Serial.print("Buttons:\t");
  Serial.println(data[MOUSE_BUTTONS], HEX);
  Serial.print("X Delta:\t");
  Serial.println((int8_t)data[X_DELTA]);
  Serial.print("Y Delta:\t");
  Serial.println((int8_t)data[Y_DELTA]);
  Serial.print("Scroll Delta:\t");
  Serial.println((int8_t)data[SCROLL_DELTA]);
  Serial.print("Pan Delta:\t");
  Serial.println((int8_t)data[PAN_DELTA]);
  Serial.println();
}

void printKeyboardData(uint8_t * data)
{
  Serial.print("Length:\t");
  Serial.println(((uint16_t)data[LENGTH_HIGHBYTE]) << 8 | (uint16_t)data[LENGTH_LOWBYTE]);
  Serial.print("Report ID:\t");
  Serial.println(data[REPORT_ID], HEX);
  Serial.print("Modifier Keys:\t");
  Serial.println(data[MODIFIER_KEYS], HEX);
  Serial.print("RESERVED:\tN/A");
  Serial.print("Keycode 1:\t");
  Serial.println(data[KEYCODE_1], HEX);    
  Serial.print("Keycode 2:\t");
  Serial.println(data[KEYCODE_2], HEX);   
  Serial.print("Keycode 3:\t");
  Serial.println(data[KEYCODE_3], HEX);   
  Serial.print("Keycode 4:\t");
  Serial.println(data[KEYCODE_4], HEX);   
  Serial.print("Keycode 5:\t");
  Serial.println(data[KEYCODE_5], HEX);  
  Serial.print("Keycode 6:\t");
  Serial.println(data[KEYCODE_6], HEX);   
  Serial.println();
}

void printAbsoluteData(uint8_t * data)
{
  Serial.print("Length:\t");
  Serial.println(((uint16_t)data[LENGTH_HIGHBYTE]) << 8 | (uint16_t)data[LENGTH_LOWBYTE]);
  Serial.print("Report ID:\t");
  Serial.println(data[REPORT_ID], HEX);   
  Serial.print("Contacts Mask:\t");
  Serial.println(data[CONTACTS_MASK], BIN);
  Serial.print("Buttons:\t");
  Serial.println(data[ABSOLUTE_BUTTONS], HEX);
      
  Serial.print("Finger 0 Palm:\t");
  Serial.println(data[FINGER_0_PALM], HEX);
  Serial.print("Finger 0 X:\t");
  Serial.println((uint16_t)data[FINGER_0_XLOW] | (uint16_t)(data[FINGER_0_XHIGH] << 8));
  Serial.print("Finger 0 Y:\t");
  Serial.println((uint16_t)data[FINGER_0_YLOW] | (uint16_t)(data[FINGER_0_YHIGH] << 8));

  Serial.print("Finger 1 Palm:\t");
  Serial.println(data[FINGER_1_PALM], HEX);
  Serial.print("Finger 1 X:\t");
  Serial.println((uint16_t)data[FINGER_1_XLOW] | (uint16_t)(data[FINGER_1_XHIGH] << 8));
  Serial.print("Finger 1 Y:\t");
  Serial.println((uint16_t)data[FINGER_1_YLOW] | (uint16_t)(data[FINGER_1_YHIGH] << 8));
  
  Serial.print("Finger 2 Palm:\t");
  Serial.println(data[FINGER_2_PALM], HEX);
  Serial.print("Finger 2 X:\t");
  Serial.println((uint16_t)data[FINGER_2_XLOW] | (uint16_t)(data[FINGER_2_XHIGH] << 8));
  Serial.print("Finger 2 Y:\t");
  Serial.println((uint16_t)data[FINGER_2_YLOW] | (uint16_t)(data[FINGER_2_YHIGH] << 8));
  
  Serial.print("Finger 3 Palm:\t");
  Serial.println(data[FINGER_3_PALM], HEX);
  Serial.print("Finger 3 X:\t");
  Serial.println((uint16_t)data[FINGER_3_XLOW] | (uint16_t)(data[FINGER_3_XHIGH] << 8));
  Serial.print("Finger 3 Y:\t");
  Serial.println((uint16_t)data[FINGER_3_YLOW] | (uint16_t)(data[FINGER_3_YHIGH] << 8));
  
  Serial.print("Finger 4 Palm:\t");
  Serial.println(data[FINGER_4_PALM], HEX);
  Serial.print("Finger 4 X:\t");
  Serial.println((uint16_t)data[FINGER_4_XLOW] | (uint16_t)(data[FINGER_4_XHIGH] << 8));
  Serial.print("Finger 4 Y:\t");
  Serial.println((uint16_t)data[FINGER_4_YLOW] | (uint16_t)(data[FINGER_4_YHIGH] << 8));

  Serial.println();
}
