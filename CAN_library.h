#include "mbed.h"
#include "SD_DISCO_F469NI.h"
#include "EEPROM_DISCO_F469NI.h"


//CAN Identificators
#define DTA_1   0x2000
#define DTA_2   0x2001
#define DTA_3   0x2002
#define DTA_4   0x2003
#define DTA_5   0x2004
#define DTA_6   0x2005
// This IDs can be changed as they are our own IDs
#define LVDT_FRONT  0x1006
#define LVDT_REAR   0x1007
#define BRAKES  0x1008
#define DISTANCE 0x1009
// If we add some other IDs in the vehicle add them here

#define DISTANCE_MEMORY_ADDR        3592960                            //Start address in SD card memory reserved for Distance Statistics.

//List of functions

void CANMsgReceive();                                    // CAN RX Interrupt Function

void UpdateInfo();                                       // Update info for DTA values

uint16_t * AllocData(CANMessage msg);                    // Unpack CAN message 

void InitDistanceMeter(uint16_t newData[4]);             // Initialise Distance Meter

//Update values
void UpdateInfoDTA1(uint16_t newData[4]);                // Update values for DTA1 message
void UpdateInfoDTA2(uint16_t newData[4]);                // Update values for DTA2 message
void UpdateInfoDTA3(uint16_t newData[4]);                // Update values for DTA3 message
void UpdateInfoDTA4(uint16_t newData[4]);                // Update values for DTA4 message
void UpdateInfoDTA5(uint16_t newData[4]);                // Update values for DTA5 message
void UpdateInfoDTA6(uint16_t newData[4]);                // Update values for DTA6 message
//void UpdateMeterCounter();            //U izradi
void send(uint16_t ID, char dta[8]);
void UpdateInfoLVDTFront(uint16_t newData[4]);           // Update values for LVDTFront message
void UpdateInfoLVDTRear(uint16_t newData[4]);            // Update values for LVDTRear message
void UpdateInfoBrakes(uint16_t newData[4]);              // Update values for Brakes message