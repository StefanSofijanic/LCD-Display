/*#include "mbed.h"
#include "functions.h"

LCD_DISCO_F469NI lcd;
SD_DISCO_F469NI sd;
Serial pc(USBTX, USBRX);
CAN can(PB_5, PB_13);
DigitalIn changescreen_command(D0);

int can_data;
int RPM;
extern GEAR *Gears;
extern IMAGE LogoBig,LogoSmall,Branko;

void MainScreen();
void FirstAuxScreen();
void SecondAuxScreen();

int main(){
    DrawRGBImage(LogoBig,0,0);
    wait(0.5);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    MainScreen();
};
    
    
void MainScreen(){
    lcd.SetTextColor(LCD_COLOR_BLACK);
    DrawSpeedMeter();
    DrawRGBImage(LogoSmall,LogoSmallXPos,LogoSmallYPos);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(30,170,(uint8_t*)"Oil Temp",LEFT_MODE);
    lcd.DisplayStringAt(30,320,(uint8_t*)"Water Temp",LEFT_MODE);
    lcd.DisplayStringAt(584,170,(uint8_t*)"TPS",LEFT_MODE);
    lcd.SetTextColor(LCD_COLOR_DARKRED);
    lcd.FillRect(560,340,230,68);
    lcd.SetBackColor(LCD_COLOR_DARKRED);
    PrintString("BRAKE",50,575,350,LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_WHITE);

    int Vin,V0=0,OilTemp,OilTemp0=0,WaterTemp,WaterTemp0=0,TPS,TPS0=0,Brake,Brake0=0,Gear,Gear0;
    
    CANMessage msg;
    
    //inicijalne vrednosti prikazane na ekranu
    PrintChar(Gears[0],GearXPos,GearYPos,LCD_COLOR_BLACK);
    ChangeNumber(0,111,100,OilTempXPos,OilTempYPos);
    ChangeNumber(0,111,100,WaterTempXPos,WaterTempYPos);
    ChangeNumber(0,111,100,TPSXPos,TPSYPos);
    BrakeSignal(0,1);
    wait(1);
    while(changescreen_command.read()==0){
        if(can.read(msg)){
            can_data=msg.data[0];
            lcd.SetTextColor(LCD_COLOR_BLACK);
            if (msg.id==SPEED_ID){
                Vin=(int)can_data;
                UpdateSpeedMeter(V0,Vin-V0);
                V0=Vin;
            }else if (msg.id==GEAR_ID){
                Gear=can_data;
                if(Gear!=Gear0){
                    Gear=can_data;
                    printf("%d\n",Gear);
                    PrintChar(Gears[Gear],GearXPos,GearYPos,LCD_COLOR_BLACK);
                    Gear0=Gear;
                };                    
            }else if (msg.id==OIL_TEMP_ID){
                OilTemp=can_data;
                ChangeNumber(OilTemp,OilTemp0,100,OilTempXPos,OilTempYPos);
                OilTemp0=OilTemp;
            }else if (msg.id==WATER_TEMP_ID){
                WaterTemp=can_data;
                ChangeNumber(WaterTemp,WaterTemp0,100,WaterTempXPos,WaterTempYPos);
                WaterTemp0=WaterTemp;
            }else if (msg.id==TPS_ID){
                TPS=can_data;
                ChangeNumber(TPS,TPS0,100,TPSXPos,TPSYPos);
                TPS0=TPS;
            }else if (msg.id==BRAKE_ID){
                Brake=can_data;
                BrakeSignal(Brake,Brake0);
                Brake0=Brake;
            };
        };
    };
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    FirstAuxScreen();
    lcd.SetTextColor(LCD_COLOR_BLACK);
};

void FirstAuxScreen(){
    PrintString("Oil P",50,20,25,LCD_COLOR_BLACK);
    PrintString("MAP",50,20,100,LCD_COLOR_BLACK);
    PrintString("Air Temp",50,20,175,LCD_COLOR_BLACK);
    PrintString("Lambda",50,20,250,LCD_COLOR_BLACK);
    PrintString("Volts",50,20,325,LCD_COLOR_BLACK);
    PrintString("Crank",50,20,400,LCD_COLOR_BLACK);
    PrintString("kPa",50,470,25,LCD_COLOR_BLACK);
    PrintString("kPa",50,470,100,LCD_COLOR_BLACK);
    PrintString("C",50,470,175,LCD_COLOR_BLACK);
    PrintString("Ratio",50,470,250,LCD_COLOR_BLACK);
    PrintString("V",50,470,325,LCD_COLOR_BLACK);
    DrawRGBImage(LogoSmall,LogoSmallXPos,LogoSmallYPos);
    
    uint8_t OilP,OilP0=0,MAP,MAP0=0,AirTemp,AirTemp0=0,Lambda,Lambda0=0,Volts,Volts0=0,Crank,Crank0=1;
    
    CANMessage msg;
    
    ChangeNumber(0,111,50,OilPXPos,OilPYPos);
    ChangeNumber(0,111,50,MAPXPos,MAPYPos);
    ChangeNumber(0,111,50,AirTempXPos,AirTempYPos);
    ChangeNumber(0,111,50,LambdaXPos,LambdaYPos);
    ChangeNumber(0,111,50,VoltsXPos,VoltsYPos);
        
    while(changescreen_command.read()==0){
        if(can.read(msg)){
            can_data=msg.data[0];
            lcd.SetTextColor(LCD_COLOR_BLACK);
            if (msg.id==OIL_P_ID){
                OilP=can_data;
                ChangeNumber(OilP,OilP0,100,OilPXPos,OilPYPos);
                OilP0=OilP;
            }else if (msg.id==MAP_ID){
                MAP=can_data;
                ChangeNumber(MAP,MAP0,100,MAPXPos,MAPYPos);
                MAP0=MAP;
            }else if (msg.id==AIR_TEMP_ID){
                AirTemp=can_data;
                ChangeNumber(AirTemp,AirTemp0,100,AirTempXPos,AirTempYPos);
                AirTemp0=AirTemp;
            }else if (msg.id==LAMBDA_ID){
                Lambda=can_data;
                ChangeNumber(Lambda,Lambda0,100,LambdaXPos,LambdaYPos);
                Lambda0=Lambda;
            }else if (msg.id==VOLTS_ID){
                Volts=can_data;
                ChangeNumber(Volts,Volts0,100,VoltsXPos,VoltsYPos);
                Volts0=Volts;
            }else if (msg.id==CRANK_ID){
                Crank=can_data;
                if(Crank!=Crank0){
                    if(Crank==1){
                        PrintString("ERROR",50,CrankXPos,CrankYPos,LCD_COLOR_RED);
                    }else if(Crank==0){
                        PrintString("OK",50,CrankXPos,CrankYPos,LCD_COLOR_RED); 
                    };
                };   
                Crank0=Crank;
            };
        };
    };
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    SecondAuxScreen();

};

void SecondAuxScreen(){
    PrintString("LVDT Sensors",50,200,20,LCD_COLOR_BLACK);
    DrawRGBImage(LogoSmall,LogoSmallXPos,LogoSmallYPos);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(FLLVDTBarXPos+25,FLLVDTBarYPos,(uint8_t*)"FL",LEFT_MODE);
    lcd.DisplayStringAt(FRLVDTBarXPos+25,FRLVDTBarYPos,(uint8_t*)"FR",LEFT_MODE);
    lcd.DisplayStringAt(BLLVDTBarXPos+25,BLLVDTBarYPos,(uint8_t*)"BL",LEFT_MODE);
    lcd.DisplayStringAt(BRLVDTBarXPos+25,BRLVDTBarYPos,(uint8_t*)"BR",LEFT_MODE);
    
    uint8_t FLLVDT,FLLVDT0=0,FRLVDT,FRLVDT0=0,BLLVDT,BLLVDT0=0,BRLVDT,BRLVDT0=0;    
    
    CANMessage msg;
    
    ChangeNumber(0,111,100,FLLVDTXPos,FLLVDTYPos);
    ChangeNumber(0,111,100,FRLVDTXPos,FRLVDTYPos);
    ChangeNumber(0,111,100,BLLVDTXPos,BLLVDTYPos);
    ChangeNumber(0,111,100,BRLVDTXPos,BRLVDTYPos);
    
    while(changescreen_command.read()==0){
        if(can.read(msg)){
            can_data=msg.data[0];
            if(msg.id==FL_LVDT_ID){
                FLLVDT=can_data;
                ChangeNumber(FLLVDT,FLLVDT0,100,FLLVDTXPos,FLLVDTYPos);
                UpdateLVDTScale(FLLVDT,FLLVDT0,FLLVDTBarXPos,FLLVDTBarYPos);
                FLLVDT0=FLLVDT;
            }else if(msg.id==FR_LVDT_ID){
                FRLVDT=can_data;
                ChangeNumber(FRLVDT,FRLVDT0,100,FRLVDTXPos,FRLVDTYPos);
                UpdateLVDTScale(FRLVDT,FRLVDT0,FRLVDTBarXPos,FRLVDTBarYPos);
                FRLVDT0=FRLVDT;
            }else if(msg.id==BL_LVDT_ID){
                BLLVDT=can_data;
                ChangeNumber(BLLVDT,BLLVDT0,100,BLLVDTXPos,BLLVDTYPos);
                UpdateLVDTScale(BLLVDT,BLLVDT0,BLLVDTBarXPos,BLLVDTBarYPos);
                BLLVDT0=BLLVDT;
            }else if(msg.id==BR_LVDT_ID){
                BRLVDT=can_data;
                ChangeNumber(BRLVDT,BRLVDT0,100,BRLVDTXPos,BRLVDTYPos);
                UpdateLVDTScale(BRLVDT,BRLVDT0,BRLVDTBarXPos,BRLVDTBarYPos);
                BRLVDT0=BRLVDT;
            };
        };
    };
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    MainScreen();
};*/