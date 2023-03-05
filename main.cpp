#include "CAN_library.h"
#include "screen_library.h"
#include "EEPROM_DISCO_F469NI.h"


LCD_DISCO_F469NI lcd;                       //Initialize LCD Display
SD_DISCO_F469NI sd;                         //Initialize SD Card
//EEPROM_DISCO_F469NI eep;                    //Initialize EEPROM Internal memory
Serial pc(USBTX, USBRX);                    //Initialize Serial. This is used only while debuging code.
CAN can(PB_5, PB_13,1000000);                       //Initialize CAN.
I2C i2c(D14, D15);
DigitalOut reset(D8);
InterruptIn change_screen_input(D0);        //Initialize Digital input for Change screen button
Timer t_int,t_store;

// Variables received from DTA, LVDTs and brakes
uint16_t Rpm0=0,Speed0=0,Gear0=0,Water_Temp0=0,Oil_Temp0=0,TPS0=0,Brakes0=0,MAP0=0,Air_Temp0=0,Lambda0=0,Volts0=0;
uint16_t Rpm=0,Speed=0,Gear=0,Water_Temp=0,Oil_Temp=0,TPS=0,Brakes=0,MAP=0,Air_Temp=0,Lambda=0,Volts=0;
int FL_LVDT0=0,FR_LVDT0=0,RL_LVDT0=0,RR_LVDT0=0,FL_LVDT=0,FR_LVDT=0,RL_LVDT=0,RR_LVDT=0;
//Referrent LVDT values. First received value is referrent.
int FL_LVDT_Ref,FR_LVDT_Ref,RL_LVDT_Ref,RR_LVDT_Ref;
//float Meter_counter=0,Meter_counter0=0;

uint8_t change_flag=0;
uint16_t rx_flag=0x0000;        // Receive specific CAN data message
uint8_t distance_flag=0;
uint8_t lvdtref=0x0F;           // Flag if refferent LVDT value is received (first received LVDT value, 1=no, 0=yes). From highest to lowest bit: LL,LR,RL,RR.
uint8_t screen_flag=0x01;       // Current screen flag. 1=Main, 2=First Aux, 3=Second Aux.
uint8_t ft_main_flag=1;         // Detect first time Main screne in loop
uint8_t ft_2aux_flag=1;         // Detect first time Second Aux screne in loop

// CAN Message variables, one variable for each ID
// If new IDs are added, add variables for them
CANMessage msgDTA1;         // RPM, TPS %, Water temp C, Air temp C
CANMessage msgDTA2;         // MAP Kpa, Lambda x1000, KPH x10, Oil P Kpa
CANMessage msgDTA3;         // Fuel P Kpa, Oil temp C, Volts x10, Fuel Con. L/Hr x10
CANMessage msgDTA4;         // Gear, Advance Deg x10, Injection ms x100, Fuel Con L/100km x10
CANMessage msgDTA5;         // Ana1 mV, Ana2 mV, Ana3 mV, Cam Advance x10
CANMessage msgDTA6;         // Cam Targ x10, Cam PWM x10, Crank Errors, Cam Errors
CANMessage msgLVDTFront;    // Left, Right, Steering Wheel 
CANMessage msgLVDTRear;     // Left, Right
CANMessage msgBrakes;       // Brake system preassure, Braking On/Off
CANMessage msgDistance;     // Total distance

// LED Bar for RPM

char cmd[3];
int addr = 0x74<<1;

void LEDInitialize(){
    reset.write(1);
    // set port as output
    cmd[0]=0x06;
    cmd[1]=0x00;
    cmd[2]=0x00;
    i2c.write(addr,cmd,3);
}

void showLedRpm(int RPM){
    pc.printf("%d\n",RPM);
    if(RPM<1000) {
        cmd[1]=0xFF;
        cmd[2]=0xFF;
        }
    else if (RPM<2000) {
        cmd[1]=0xFF;
        cmd[2]=0xDF;
        }
    else if (RPM <3000) {
        cmd[1]=0xFF;
        cmd[2]=0xCF;
        }
    else if (RPM <4000) {
        cmd[1]=0xFF;
        cmd[2]=0xC7;
        }
    else if (RPM <5000) {
        cmd[1]=0xFF;
        cmd[2]=0xC3;
        }
    else if (RPM <6000) {
        cmd[1]=0xFF;
        cmd[2]=0xC1;
        }
    else if (RPM <7000) {
        cmd[1]=0xFF;
        cmd[2]=0xC0;
        }
    else if (RPM <8000) {
        cmd[1]=0xBF;
        cmd[2]=0xC0;
        }
    else if (RPM <9000) {
        cmd[1]=0x9F;
        cmd[2]=0xC0;
        }
    else if (RPM <10000) {
        cmd[1]=0x8F;
        cmd[2]=0xC0;
        }
    else if (RPM <11000) {
        cmd[1]=0x87;
        cmd[2]=0xC0;
        }
    else if (RPM <12000) {
        cmd[1]=0x83;
        cmd[2]=0xC0;
        }
    else if (RPM <13000) {
        cmd[1]=0x81;
        cmd[2]=0xC0;
        }
    else {
        cmd[1]=0x80;
        cmd[2]=0xC0;
        }
    cmd[0]=0x02;
    i2c.write(addr, cmd, 3);
}


int main(){ 
    LEDInitialize();   
    can.attach(&CANMsgReceive,CAN::RxIrq);      // Attach interrupt function to CAN RX
    change_screen_input.rise(&ChangeCommand);   //Attach interrupt function to rising edge of DigitalIn for changing screen.
    SetIntro();                                 // Display logo when starting display
    SetMain();                                  // First screen is main by default
    sd.Init();
    
    while(1){
        if(change_flag){                        // Check if screen is changed
            ChangeScreen();                         
        };
        UpdateInfo();                           // Update info for DTA values in every iteration
        switch(screen_flag){                    // Display only changes visible on current screen
            case(1):
                MainUpdate();
                break;
            case(2):
                FirstAuxUpdate();
                break;
            case(3):
                SecondAuxUpdate();
                break;
        };
    };
    
};