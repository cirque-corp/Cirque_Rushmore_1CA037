#ifndef API_C2_H
#define API_C2_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license
/**@file API_C2.h
   @brief This file contains the API for working with the Gen4 firmware */
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "API_HostBus.h"
#include "API_Hardware.h"

/***********************************************************/
/***********************************************************/
/******************* PROVIDED DEFINES **********************/

/** Report IDs */
#define MOUSE_REPORT_ID         (0x06) /**< ID of a Mouse Report */
#define KEYBOARD_REPORT_ID      (0x08) /**< ID of a Keyboard Report */
#define CRQ_ABSOLUTE_REPORT_ID  (0x09) /**< ID of a Cirque Absolute Report */

/** Keyboard Modifier Masks see keyboardReport_t */
#define KEYBOARD_MODIFIER_LEFT_CTRL_KEY_MASK    0x01
#define KEYBOARD_MODIFIER_LEFT_SHIFT_KEY_MASK   0x02
#define KEYBOARD_MODIFIER_LEFT_ALT_KEY_MASK     0x04
#define KEYBOARD_MODIFIER_LEFT_GUI_KEY_MASK     0x08
#define KEYBOARD_MODIFIER_RIGHT_CTRL_KEY_MASK   0x10
#define KEYBOARD_MODIFIER_RIGHT_SHIFT_KEY_MASK  0x20
#define KEYBOARD_MODIFIER_RIGHT_ALT_KEY_MASK    0x40
#define KEYBOARD_MODIFIER_RIGHT_GUI_KEY_MASK    0x80 

/** Palm Data Masks see fingerData_t */
#define CRQ_ABSOLUTE_PALM_REJECT_MASK   0x80
#define CRQ_ABSOLUTE_CONFIDENCE_MASK    0x02
#define CRQ_ABSOLUTE_SINGLE_SAMPLE_MASK 0x01

/** Button Masks see mouseReport_t, CRQabsoluteReport_t */
#define BUTTON_1_MASK 0x1   /** Left */
#define BUTTON_2_MASK 0x2   /** Right */
#define BUTTON_3_MASK 0x4   /** Middle */
#define BUTTON_8_MASK 0x80  /** Touchpad button */

 /** Register definition */
#define REG_CHIP_ID             (0xC2C0)
#define REG_FIRMWARE_VER        (0xC2C1) 
#define REG_FIRMWARE_SUBVERSION (0xC2DC) 
#define REG_VENDOR_ID           (0xC2D4)
#define REG_PRODUCT_ID          (0xC2D6)
#define REG_VERSION_ID          (0xC2D8)

/** length of cirque absolute report packet 
	(largest packet size) */
#define PACKET_SIZE 53

/***********************************************************/
/***********************************************************/
/****************** PUBLIC DATA STRUCTURES *****************/

typedef struct
{
    uint16_t vendorId;           /**< HID VID */
    uint16_t productId;          /**< HID PID */
    uint16_t versionId;          /**< HID version ID */
    uint8_t  chipId;             /**< Identifies the HW platform this firmware is designed for */
    uint8_t  firmwareVersion;    /**< Identifies the FW version that is running on the system */
    uint8_t  firmwareSubversion; /**< Provides granular information about which firmware is running */
    
} systemInfo_t;  

/** Contains the data from a mouse report packet.*/
typedef struct 
{
    uint8_t buttons;     /**< Bitmap of the button states */
    int8_t  xDelta;      /**< Change in Horizontal movement */
    int8_t  yDelta;      /**< Change in vertical movement */
    int8_t  scrollDelta; /**< Vertical Scroll value */
    int8_t  panDelta;    /**< Horizontal Scroll (or Pan) value */
} mouseReport_t; 

/** Contains the data for a Keyboard report */
typedef struct 
{
    uint8_t keycode[6]; /**< Keycodes pressed, only the first one is used for now */
    uint8_t modifier;   /**< alt,ctrl, gui keys */
} keyboardReport_t;

/** Subcontainer for holding the Cirque Absolute Report data of a single finger*/
typedef struct 
{
    uint16_t x;    /** < Absolute X position of finger */
    uint16_t y;    /**< Absolute Y position of finger */
    uint8_t  palm; /** < Bitfield with Palm-reject, confidence and single sample information Note: a finger may have old or inaccurate x,y data when the confidence is low*/
} fingerData_t; 

/** Contains the Report data for the Cirque Absolute mode (report id 9) */
typedef struct 
{
    fingerData_t fingers[5];   /**< Array of 5 Fingers */
    uint8_t      contactFlags; /**< bitmap of contacted fingers ie. 0x3 means fingers 0 and 1 are down*/
    uint8_t      buttons;      /**< Bitmap of the button states */
} CRQabsoluteReport_t; 

/** Struct that describes a generic report.
    This is a convienent container for handling report data.
    Use the reportID to know which member of the Union to use.*/
typedef struct 
{
	/** This union allows the report to be generic.*/	
    union
    {
        CRQabsoluteReport_t abs;   /**< Treats the data as an absolute report */
        keyboardReport_t keyboard; /**< Treats the data as a keyboard report */
        mouseReport_t mouse;       /**< Treat the data as a mouse report */
    };
    uint8_t reportID; /**< ID of the report. Shows what type of report to use */
} report_t; 

/***********************************************************/
/***********************************************************/
/******************* IMPORTANT FUNCTIONS *******************/

void API_C2_init(int32_t I2CFrequency, uint8_t I2CAddress);

bool API_C2_DR_Asserted(void); 

void API_C2_getReport(report_t* result);

uint8_t API_C2_readRegister(uint32_t address); 

void API_C2_writeRegister(uint32_t address, uint8_t value);

void API_C2_readSystemInfo(systemInfo_t* result);

/***********************************************************/
/***********************************************************/
/************************* ACTIONS *************************/  

void API_C2_setCRQ_AbsoluteMode(void);

void API_C2_setRelativeMode(void);

void API_C2_persistToFlash(void); 

void API_C2_enableComp(void);

void API_C2_disableComp(void);

void API_C2_forceComp(void);

bool API_C2_factoryCalibrate(void); 

void API_C2_disableFeed(void);

void API_C2_enableFeed(void);

void API_C2_disableTracking(void);

void API_C2_enableTracking(void);

/***********************************************************/
/***********************************************************/
/*********** TOOLS FOR DETERMINIG INPUT EVENTS *************/

bool API_C2_isFingerValid(report_t* report, uint8_t finger_num);

bool API_C2_isFingerContacted(report_t* report, uint8_t finger_num);

// uint8_t API_C2_numberFingers(report_t* report); no definition

bool API_C2_isButtonPressed(report_t* report, uint8_t buttonMask);

/***********************************************************/
/***********************************************************/
/***********************************************************/

void API_C2_decodeReport(uint8_t* packet, report_t* result);

#ifdef __cplusplus
}
#endif

#endif // API_C2_H