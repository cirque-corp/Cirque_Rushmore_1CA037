#include "Gen4.h"

//  Hardware Interface
//  GND
//  +3.3V
//  SDA = Pin 18  (there MUST be a pull-up to 3.3V on this signal; 4.7k recommended)
//  SCL = Pin 19  (there MUST be a pull-up to 3.3V on this signal; 4.7k recommended)
//  DR = Pin 20

#define DR_PIN    20

uint8_t gen4Buffer[53];
uint32_t zIdleCount = 0;

// setup() gets called once at power-up
void setup()
{  
  Serial.begin(115200);
  delay(750);
  Serial.println("Running Gen4 I2C Demo...");
  
  Gen4_init();
  
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
      default:
        Serial.println("Invalid input...");
        break;
    }
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

