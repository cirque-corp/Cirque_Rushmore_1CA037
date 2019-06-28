// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

#include "API_Gen4.h"
#include "API_HostBus.h"
#include "API_Hardware.h"

#define NULL 0
#define PACKET_SIZE 53 //length of cirque absolute report packet (largest packet size)

/*******HELPER FUNCTION DELCARATIONS *******/
void decodeReport(uint8_t* packet, report_t* result);
void decodeMouseReport(uint8_t* packet, report_t* result);
void decodeKeyboardReport(uint8_t* packet, report_t* result);
void decodeCirqueAbsoluteReport(uint8_t* packet, report_t* result);
void clearReport(report_t* report);
uint16_t read16bitRegister(uint32_t address);
/*******************************************/

void API_Gen4_init(int32_t I2CFrequency, uint8_t I2CAddress)
{
    HB_init(I2CFrequency, I2CAddress);
}

bool API_Gen4_DR_Asserted(void)
{
    return (HB_DR_Asserted()); 
}

void API_Gen4_getReport(report_t* result)
{
    //read packet
    uint8_t packet[PACKET_SIZE]; 
    HB_readReport(packet, PACKET_SIZE); //read_report fills packet with i2c packet
    API_Gen4_decodeReport(packet, result);
}

uint8_t API_Gen4_readRegister(uint32_t address)
{
    uint8_t contents = 0;
    HB_readExtendedMemory(address, &contents, 1);
    return contents;
}

uint16_t read16bitRegister(uint32_t address)
{
    uint8_t contents[2] = {0,0};
    
    HB_readExtendedMemory(address, contents, 2);
    uint16_t result = (uint16_t) contents[0] | (contents[1] << 8);
    return result;
}

void API_Gen4_writeRegister(uint32_t address, uint8_t value)
{
    HB_writeExtendedMemory(address, &value, 1);
}

#define REG_CHIP_ID             (0xC2C0)
#define REG_FIRMWARE_VER        (0xC2C1) 
#define REG_FIRMWARE_SUBVERSION (0xC2DC) 
#define REG_VENDOR_ID           (0xC2D4)
#define REG_PRODUCT_ID          (0xC2D6)
#define REG_VERSION_ID          (0xC2D8)

void API_Gen4_readSystemInfo(systemInfo_t* result)
{
    result->chipId = API_Gen4_readRegister(REG_CHIP_ID);
    result->firmwareVersion = API_Gen4_readRegister(REG_FIRMWARE_VER);
    result->firmwareSubversion = API_Gen4_readRegister(REG_FIRMWARE_SUBVERSION);
    result->vendorId = read16bitRegister(REG_VENDOR_ID);
    result->productId = read16bitRegister(REG_PRODUCT_ID);
    result->versionId = read16bitRegister(REG_VERSION_ID);
}

void API_Gen4_forceComp()
{
    uint8_t feedConfig1 = API_Gen4_readRegister(0xC2C4);  // read
    feedConfig1 |= 0x80;                                  // modify
    API_Gen4_writeRegister(0xC2C4, feedConfig1);          // write
}

void API_Gen4_setCRQ_AbsoluteMode()
{
    uint8_t feedConfig1 = API_Gen4_readRegister(0xC2C4);    // read
    feedConfig1 |= 0x02;                                    // modify
    API_Gen4_writeRegister(0xC2C4, feedConfig1);            // write
}

void API_Gen4_setRelativeMode()
{
    uint8_t feedConfig1 = API_Gen4_readRegister(0xC2C4);    // read
    feedConfig1 &= ~0x02;                                   // modify
    API_Gen4_writeRegister(0xC2C4, feedConfig1);            // write
}

void API_Gen4_persistToFlash()
{
    uint8_t persistentDataControl = API_Gen4_readRegister(0xC2DF);  // read
    persistentDataControl |= 0x01;                                  // modify
    API_Gen4_writeRegister(0xC2DF, persistentDataControl);          // write
}    

void API_Gen4_enableComp()
{
    uint8_t compConfig = API_Gen4_readRegister(0xC2C7);             // read
    compConfig |= 0x3E;                                             // modify
    API_Gen4_writeRegister(0xC2C7,compConfig);                      // write
}

void API_Gen4_disableComp()
{
    uint8_t compConfig = API_Gen4_readRegister(0xC2C7);             // read
    compConfig &= ~0x3E;                                            // modify
    API_Gen4_writeRegister(0xC2C7,compConfig);                      // write
}

bool API_Gen4_factoryCalibrate()
{
    uint8_t previousValue = API_Gen4_readRegister(0xC2C4);          // read
    uint8_t modifiedValue = previousValue | 0x80;                   // modify
    API_Gen4_writeRegister(0xC2C4, modifiedValue);                  // write
    
    API_Hardware_delay(20);
    
    uint32_t timeout = 0; // times out at 5e6 count.  ~150ms- ish
    while(API_Gen4_readRegister(0xC2C4) == modifiedValue && (timeout < 5000000) ) timeout++;
    if(timeout == 500000)
    {
        return false;
    }
    API_Gen4_writeRegister(0xC2DF, 0x03);                   // write
    
    timeout = 0;  // times out at 5e6 count.  ~150ms- ish
    while(API_Gen4_readRegister(0xC2D4) != 0x00 && (timeout < 5000000)) timeout++;
    if(timeout == 500000)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void API_Gen4_enableFeed()
{
    uint8_t feedConfig1 = API_Gen4_readRegister(0xC2C4);       // read
    feedConfig1 |= 0x01;                                       // modify
    API_Gen4_writeRegister(0xC2C4, feedConfig1);               // write
}

void API_Gen4_disableFeed()
{
    uint8_t feedConfig1 = API_Gen4_readRegister(0xC2C4);       // read
    feedConfig1 &= ~0x01;                                      // modify
    API_Gen4_writeRegister(0xC2C4, feedConfig1);               // write
}

void API_Gen4_disableTracking()
{
    uint8_t sysConfig1 = API_Gen4_readRegister(0xC2C2);         // read
    sysConfig1 &= ~0x02;                                        // modify
    API_Gen4_writeRegister(0xC2C2, sysConfig1);                 // write
}

void API_Gen4_enableTracking()
{
    uint8_t sysConfig1 = API_Gen4_readRegister(0xc2c2);         // read
    sysConfig1 |= 0x02;                                         // modify
    API_Gen4_writeRegister(0xC2C2, sysConfig1);                 // write
}

/** determines if the finger_num finger corresponds to a valid finger in the report */
bool API_Gen4_isFingerValid(report_t* report, uint8_t finger_num)
{
    if(finger_num > 4 || finger_num < 0 )
    {
        return false;
    }
    
    if(report == NULL || report->reportID != CRQ_ABSOLUTE_REPORT_ID)
    {
        return false;
    }
    
    uint8_t palm = report->abs.fingers[finger_num].palm;
    return ((palm & CRQ_ABSOLUTE_CONFIDENCE_MASK) && !(palm &CRQ_ABSOLUTE_PALM_REJECT_MASK));
}

/** determines if the finger_num is contacted */
bool API_Gen4_isFingerContacted(report_t* report, uint8_t finger_num)
{
    if(finger_num > 4 || finger_num < 0 )
    {
        return false;
    }
    
    if(report == NULL || report->reportID != CRQ_ABSOLUTE_REPORT_ID)
    {
        return false;
    }
    return report->abs.contactFlags &  (0x1 << finger_num); 
}

bool API_Gen4_isButtonPressed(report_t* report, uint8_t buttonMask)
{
    uint8_t buttons = 0;
    if(report == NULL)
        return false;
    switch(report->reportID)
    {
        case MOUSE_REPORT_ID:
            buttons = report->mouse.buttons;
            break;
        case CRQ_ABSOLUTE_REPORT_ID:
            buttons = report->abs.buttons;
            break;
        default:
            buttons = 0;
    }
    return buttons & buttonMask;
}

void API_Gen4_decodeReport(uint8_t* packet, report_t* result)
{
    result->reportID = packet[2];
    //determine which type of report it is and decode it
    switch(result->reportID) //packet id
    {
        case MOUSE_REPORT_ID:
            decodeMouseReport(packet, result);
            break;
        case KEYBOARD_REPORT_ID:
            decodeKeyboardReport(packet, result);
            break;
        case CRQ_ABSOLUTE_REPORT_ID:
            decodeCirqueAbsoluteReport(packet, result);
            break;
        default:
            clearReport(result); //return an empty report
            break;
        
    }
}

/** Decodes a packet of a mouse report and puts it into result */
void decodeMouseReport(uint8_t* packet, report_t* result)
{
    uint8_t* iter = &packet[2];
    if(*iter != MOUSE_REPORT_ID)
    {
        //it's not a mouse report 
        return; 
    }

    iter++;
    result->mouse.buttons = *iter++;
    result->mouse.xDelta = (int8_t) *iter++;
    result->mouse.yDelta = (int8_t) *iter++;
    result->mouse.scrollDelta = (int8_t) *iter++;
    result->mouse.panDelta = (int8_t) *iter;
}

/** Decodes a keyboard report packet into result */
void decodeKeyboardReport(uint8_t* packet, report_t* result)
{
    uint8_t* iter = &packet[2];
    if(*iter != KEYBOARD_REPORT_ID)
    {
        //it's not a keyboard report 
        return; //probably shouldn't leave cleanly
    }
  
    iter++;
    result->keyboard.modifier = *iter++;
    iter++;             //reserved byte
    result->keyboard.keycode[0] = *iter++;
    result->keyboard.keycode[1] = *iter++;
    result->keyboard.keycode[2] = *iter++;
    result->keyboard.keycode[3] = *iter++;
    result->keyboard.keycode[4] = *iter++;
    result->keyboard.keycode[5] = *iter++;
}

void decodeCirqueAbsoluteReport(uint8_t* packet, report_t* result)
{
    uint8_t* iter = &packet[2];
    if(*iter != CRQ_ABSOLUTE_REPORT_ID)
    {
        //it's not a absolute report 
        return; 
    }
    iter++;
    result->abs.contactFlags = *iter++;
    uint8_t i;
    for(i = 0; i < 5; i++)
    {
        result->abs.fingers[i].palm = *iter++;
        
        result->abs.fingers[i].x = (uint16_t ) *iter++;
        result->abs.fingers[i].x |= (uint16_t ) *iter++ << 8;
        
        result->abs.fingers[i].y = (uint16_t ) *iter++;
        result->abs.fingers[i].y |= (uint16_t ) *iter++ << 8;
    }
    result->abs.buttons = *iter;
}

/** Clears all values of report to zero. 
    Assumes absolute report is the largest of the union*/
void clearReport(report_t* report)
{
    report->reportID = 0;
    //Clears the values of the largest member of the Union.
    report->abs.contactFlags = 0;
    uint8_t i;
    for(i = 0; i < 5; i++) // each of the 5 fingers
    {
        report->abs.fingers[i].palm = 0;
        report->abs.fingers[i].x = 0;
        report->abs.fingers[i].y = 0;
    }
    report->abs.buttons = 0;
}
