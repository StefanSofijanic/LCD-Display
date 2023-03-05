#include "LCD_DISCO_F469NI.h"
#include "SD_DISCO_F469NI.h"

#define PI  3.14159265358979323846
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"               //Pattern which converts uint8_t to binary(array of 8 chars)
#define BYTE_TO_BINARY(byte)\
  (byte & 0x80 ? '1' : '0'),\
  (byte & 0x40 ? '1' : '0'),\
  (byte & 0x20 ? '1' : '0'),\
  (byte & 0x10 ? '1' : '0'),\
  (byte & 0x08 ? '1' : '0'),\
  (byte & 0x04 ? '1' : '0'),\
  (byte & 0x02 ? '1' : '0'),\
  (byte & 0x01 ? '1' : '0')

#define LOGOBIG_START_ADDR          0                                  //Address for Big Logo in SD Card used in Intro
#define LOGOSMALL_START_ADDR        1536000                            //Address for Small Logo in SD Card used as header
#define BRANKO_START_ADDR           1576960                            //Branko start address
#define NEW_YEAR_CONGAT_START_ADDR  2056960                            //New Year start address

const double PHI=53.13010235*PI/180;                            //Angles used in drawing Speedmeter.
const double ALPHA=73.73979529*PI/180;                          //
const double Vmax=150;

//Positions of Informations on screen
const uint16_t GearXPos=272,GearYPos=95;                                  //Gear in Main
const uint16_t OilTempXPos=15,OilTempYPos=200;                            //Oil Temperature in Main
const uint16_t LogoSmallXPos=635,LogoSmallYPos=5;                         //Small Logo, all three screens
const uint16_t WaterTempXPos=15,WaterTempYPos=350;                        //Water Temperature in Main
const uint16_t TPSXPos=569,TPSYPos=200;                                   //TPS in Main
const uint16_t RpmXPos=330,RpmYPos=30;                                  //Oil Pressure in First Auxiliary
const uint16_t MAPXPos=330,MAPYPos=90;                                    //MAP in First Auxiliary
const uint16_t AirTempXPos=330,AirTempYPos=150;                           //Air Temperature in First Auxiliary
const uint16_t LambdaXPos=330,LambdaYPos=210;                             //Lambda Sensor value in First Auxiliary
const uint16_t VoltsXPos=330,VoltsYPos=270;                               //Volts in Accumulator in First Auxiliary
const uint16_t CrankXPos=330,CrankYPos=330;                               //Crank Error Message in First Auxiliary
const uint16_t Meter_counterXPos=330,Meter_counterYPos=390;               //Kilometer counter in First Auxiliary
//YPos in bottom position for LVDTs
const uint16_t FLLVDTBarXPos=280,FLLVDTBarYPos=200;                       //Front Left Bar LVDT in Second Auxiliary
const uint16_t FRLVDTBarXPos=440,FRLVDTBarYPos=200;                       //Front Right Bar LVDT in Second Auxiliary
const uint16_t RLLVDTBarXPos=280,RLLVDTBarYPos=410;                       //Rear Left Bar LVDT in Second Auxiliary
const uint16_t RRLVDTBarXPos=440,RRLVDTBarYPos=410;                       //Rear Right Bar LVDT in Second Auxiliary
const uint16_t FLLVDTXPos=10,FLLVDTYPos=100;                              //Front Left LVDT in Second Auxiliary
const uint16_t FRLVDTXPos=534,FRLVDTYPos=100;                             //Front Right LVDT in Second Auxiliary
const uint16_t RLLVDTXPos=10,RLLVDTYPos=310;                              //Rear Left LVDT in Second Auxiliary
const uint16_t RRLVDTXPos=534,RRLVDTYPos=310;                             //Rear Right LVDT in Second Auxiliary


typedef struct BWImage {                                                  //Black-White Image Structure. These are stored on the controller
    char name;
    uint16_t width;
    uint16_t height;
    uint8_t *bitmap;
} GEAR,CHAR;

typedef struct RGBImage {                                                 //RGB Coloured Image Structure. These are stored on SD Card
    uint16_t width;
    uint16_t height;
    uint32_t START_ADDR;
} IMAGE;


void DrawSpeedMeter();                                                                                                    //Draw Speedmeter function
void PrintChar(CHAR Char,uint16_t StartXPos,uint16_t StartYPos,uint32_t TextColor);                                       //Print Char function
void PrintString(char str[],int font,uint16_t StartXPos,uint16_t StartYPos,uint32_t TextColor);                           //Print String function
void ChangeNumber(int num,int num0,int Font,uint16_t StartXPos, uint16_t StartYPos, int digits, int dec_point, int sign); //Update number function
void SetNumber(int num,int Font,uint16_t StartXPos,uint16_t StartYPos, int digits, int dec_point, int sign);              //Set number to specific value function
void DrawRGBImage(IMAGE Image,uint16_t StartXPos,uint16_t StartYPos);                                                     //Draw RGB Coloured image function
void UpdateSpeedMeter(int V,int dV);                                                                                      //Update Speedmeter function
void ChangeCrank(int Crank);                                                                                              //Update Crank error message function
int UpdateLVDTScale(int H,int H0, uint16_t StartXPos, uint16_t StartYPos);                                  //Update LVDT Bar function
void BrakeSignal(uint16_t brake);                                                                                              //Set Brake signal function
void TestFont();                                                                                                          //Test font 50 function
