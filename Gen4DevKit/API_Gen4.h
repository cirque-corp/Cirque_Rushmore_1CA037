#ifndef API_Gen4_H
#define API_Gen4_H

// Copyright (c) 2018 Cirque Corp. Restrictions apply. See: www.cirque.com/sw-license

/**@file API_Gen4.h
   @brief This file contains the API for working with the Gen4 firmware 
*/
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif
/******************* PROVIDED DEFINES **********************/

// Report IDs
#define MOUSE_REPORT_ID         (0x06) /**<ID of a Mouse Report */
#define KEYBOARD_REPORT_ID      (0x08) /**<ID of a Keyboard Report */
#define CRQ_ABSOLUTE_REPORT_ID  (0x09) /**<ID of a Cirque Absolute Report */

/** Keyboard Modifier Masks 
    see keyboardReport_t */
#define KEYBOARD_MODIFIER_LEFT_CTRL_KEY_MASK    0x01
#define KEYBOARD_MODIFIER_LEFT_SHIFT_KEY_MASK   0x02
#define KEYBOARD_MODIFIER_LEFT_ALT_KEY_MASK     0x04
#define KEYBOARD_MODIFIER_LEFT_GUI_KEY_MASK     0x08
#define KEYBOARD_MODIFIER_RIGHT_CTRL_KEY_MASK   0x10
#define KEYBOARD_MODIFIER_RIGHT_SHIFT_KEY_MASK  0x20
#define KEYBOARD_MODIFIER_RIGHT_ALT_KEY_MASK    0x40
#define KEYBOARD_MODIFIER_RIGHT_GUI_KEY_MASK    0x80 

/** Palm Data Masks 
    see fingerData_t */
#define CRQ_ABSOLUTE_PALM_REJECT_MASK   0x80
#define CRQ_ABSOLUTE_CONFIDENCE_MASK    0x02
#define CRQ_ABSOLUTE_SINGLE_SAMPLE_MASK 0x01

/** Button Masks
    see mouseReport_t, CRQabsoluteReport_t */
#define BUTTON_1_MASK 0x1   //Left
#define BUTTON_2_MASK 0x2   //Right
#define BUTTON_3_MASK 0x4   //Middle
#define BUTTON_8_MASK 0x80  // Touchpad button

/****************** Public Data Structures *****************/

typedef struct
{
    uint8_t     chipId;                 /**< Identifies the HW platform this firmware is designed for */
    uint8_t     firmwareVersion;        /**< Identifies the FW version that is running on the system */
    uint8_t     firmwareSubversion;     /**< Provides granular information about which firmware is running */
    uint16_t    vendorId;               /**< HID VID */ 
    uint16_t    productId;              /**< HID PID */ 
    uint16_t    versionId;              /**< HID version ID */ 
    
} systemInfo_t;  

/** Contains the data from a mouse report packet.*/
typedef struct 
{
    uint8_t buttons;        /**< Bitmap of the button states */
    int8_t  xDelta;         /**< Change in Horizontal movement */
    int8_t  yDelta;         /**< Change in vertical movement */
    int8_t  scrollDelta;    /**< Vertical Scroll value */
    int8_t  panDelta;       /**< Horizontal Scroll (or Pan) value */
} mouseReport_t; 

/** Contains the data for a Keyboard report */
typedef struct 
{
    uint8_t modifier;   /**< alt,ctrl, gui keys */
    uint8_t keycode[6]; /**< Keycodes pressed, only the first one is used for now */
} keyboardReport_t;

/** Subcontainer for holding the Cirque Absolute Report data of a single finger*/
typedef struct 
{
    uint8_t  palm;  /** < Bitfield with Palm-reject, confidence and single sample information */
    uint16_t x;     /** < Absolute X position of finger */
    uint16_t y;     /**< Absolute Y position of finger */
} fingerData_t; 

/** Contains the Report data for the Cirque Absolute mode (report id 9) */
typedef struct 
{
    uint8_t         contactFlags;   /**< bitmap of contacted fingers ie. 0x3 means fingers 0 and 1 are down*/
    fingerData_t    fingers[5];     /**< Array of 5 Fingers */
    uint8_t         buttons;        /**< Bitmap of the button states */
} CRQabsoluteReport_t; 

/** Struct that describes a generic report.
    This is a convienent container for handling report data.
    Use the reportID to know which member of the Union to use.*/
typedef struct 
{ 
    uint8_t reportID;   /**< ID of the report. Shows what type of report to use */
    union               /**< This union allows the report to be generic.*/
    {
        mouseReport_t mouse;        /** < Treat the data as a mouse report */
        keyboardReport_t keyboard;  /**< Treats the data as a keyboard report */
        CRQabsoluteReport_t abs;    /**< Treats the data as an absolute report */
    };
} report_t; 

/**********************************************************/

/******************* IMPORTANT FUNCTIONS ******************/

/** Initializes the Host Bus I2C connection.
    Must be run prior to any API calls
    The I2C bus commonly runs at a clock frequency of 400kHz.
    The operation of touch system is also tested at 100kHz. 
*/
void API_Gen4_init(int32_t I2CFrequency, uint8_t I2CAddress);

/** Shows when data is available.*/   
bool API_Gen4_DR_Asserted(void); 

/** Reads a report from the Host Bus.
    The report is read and decoded into the result parameter.
    Call this when the DR line is asserted. 
*/
void API_Gen4_getReport(report_t* result);

/** Reads the contents of a register at a given address */
uint8_t API_Gen4_readRegister(uint32_t address); 

/** Sets register contents.
    It is best to first read a register, modify the necessary bits, then write it back
*/
void API_Gen4_writeRegister(uint32_t address, uint8_t value);

/** Reads the System info and puts it into result*/
void API_Gen4_readSystemInfo(systemInfo_t* result);

/******************************************************/

/****************** ACTIONS ***************************/    

/** Sets the report type to CRQ_ABSOLUTE Mode */
void API_Gen4_setCRQ_AbsoluteMode(void);

/** Sets the Report type to Mouse and Keyboard reporting 
    This is compliant with HID protocol.
    Gen4 Devices are in Relative mode by default.
    Wait 50ms for it to take effect. 
*/
void API_Gen4_setRelativeMode(void);

/** Stores all registers and comp into flash memory */
void API_Gen4_persistToFlash(); 

/** Enables compensation if it was disabled. 
    The Comp is enabled by default
    Wait 50ms for it to take effect. 
*/
void API_Gen4_enableComp(void);

/** Disables compensation.
    Compensation can still be forced with API_Gen4_forceComp.
    Wait 50ms for it to take effect. 
 */
void API_Gen4_disableComp(void);

/** Forces a reset of the compensation. 
    Wait 50ms for it to take effect. 
*/
void API_Gen4_forceComp(void);

/** Takes a clean compensation image in the factory
    returns true if successful, false if watchdog timeout
    This takes about 200 ms to run.
*/
bool API_Gen4_factoryCalibrate(void); 

/** Turns off the feed.
    The touchpad continues to calculate the touch data, but will stop sending DR signals
    ISSUE: Becomes renabled if awoken from sleep. 
*/
void API_Gen4_disableFeed(void);

/** Turns on the feed 
    Wait 50ms for it to take effect. 
*/
void API_Gen4_enableFeed(void);

/** Turns off tracking.
    The touchpad stops calculating touch data. 
    It then goes to sleep if power control is enabled
    Wait 50ms for it to take effect. 
*/
void API_Gen4_disableTracking(void);

/** Turns on tracking. */
void API_Gen4_enableTracking(void);

/**********************************************************/

/********** Tools for Determining Input Events ************/

/** determines if the finger_num finger corresponds to a valid finger in the report */
bool API_Gen4_isFingerValid(report_t* report, uint8_t finger_num);

/** determines if the finger_num finger is in contact with the module */
bool API_Gen4_isFingerContacted(report_t* report, uint8_t finger_num);

/** determines if a button is pressed according to its mask. 
    returns false if it's a keyboard report with no button information.*/
bool API_Gen4_isButtonPressed(report_t* report, uint8_t buttonMask);

/***********************************************************/

/** Included for testing purposes.
    API_Gen4_getReport calls this */
void API_Gen4_decodeReport(uint8_t* packet, report_t* result);


#ifdef __cplusplus
}
#endif

#endif //API_Gen4_H

