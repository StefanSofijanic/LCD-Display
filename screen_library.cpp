#include "screen_library.h"

extern LCD_DISCO_F469NI lcd;
extern SD_DISCO_F469NI sd;
extern DigitalIn change_screen_input;

extern uint8_t screen_flag,ft_main_flag,ft_2aux_flag,change_flag;
extern GEAR *Gears;
extern IMAGE LogoBig,LogoSmall,Branko,NewYearCongat;

extern uint16_t Rpm0, Speed0, Gear0, Water_Temp0, Oil_Temp0, TPS0, Brakes0, MAP0, Air_Temp0, Lambda0, Volts0, Crank0;
extern uint16_t Rpm,  Speed,  Gear,  Water_Temp,  Oil_Temp,  TPS,  Brakes,  MAP,  Air_Temp,  Lambda,  Volts,  Crank;
extern int FL_LVDT0, FR_LVDT0, RL_LVDT0, RR_LVDT0, FL_LVDT,  FR_LVDT,  RL_LVDT,  RR_LVDT;
extern int FL_LVDT_Ref,FR_LVDT_Ref,RL_LVDT_Ref,RR_LVDT_Ref;
extern int LVDT_Max;
//extern float Meter_counter,Meter_counter0;

void ChangeCommand(){                           //Interrupt function for registering change command
    change_flag=1;
};

void ChangeScreen(){                            //Changes the screen if needed.
    switch(screen_flag){                      
        case(3):
            screen_flag=1;                      //Change flag to new value.
            ft_main_flag=1;                     //Set flag which point that Main screne is entered.
            SetMain();                          //Set Main Screne.
            break;
        case(1):
            screen_flag=2;
            SetFirstAux();
            break;
        case(2):
            screen_flag=3;
            SetSecondAux();
            break;
    };
    change_flag=0;
};



void SetIntro(){                                // Set Intro screen
    DrawRGBImage(LogoBig,0,0);
    wait(1);
};

void SetMain(){                                 // Set Main screen
    //Erase previous screen
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    //Draw Main screen
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
    
    //Display initial values
    PrintChar(Gears[Gear],GearXPos,GearYPos,LCD_COLOR_BLACK);
    SetNumber(Oil_Temp,100,OilTempXPos,OilTempYPos,3,0,0);
    SetNumber(Water_Temp,100,WaterTempXPos,WaterTempYPos,3,0,0);
    SetNumber(TPS,100,TPSXPos,TPSYPos,3,0,0);
    BrakeSignal(Brakes);
};


void SetFirstAux(){                         // Set First Auxiliary screen
    // Erase previous screen
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    //Draw First Auxiliary screen
    PrintString("RPM",50,20,30,LCD_COLOR_BLACK);
    PrintString("MAP",50,20,90,LCD_COLOR_BLACK);
    PrintString("Air Temp",50,20,150,LCD_COLOR_BLACK);
    PrintString("Lambda",50,20,210,LCD_COLOR_BLACK);
    PrintString("Volts",50,20,270,LCD_COLOR_BLACK);
    PrintString("Distance:",50,20,330,LCD_COLOR_BLACK);
    //PrintString("",50,470,30,LCD_COLOR_BLACK);
    PrintString("kPa",50,560,90,LCD_COLOR_BLACK);
    PrintString("C",50,560,150,LCD_COLOR_BLACK);
    PrintString("Ratio",50,560,210,LCD_COLOR_BLACK);
    PrintString("V",50,560,270,LCD_COLOR_BLACK);
    PrintString("km",50,560,330,LCD_COLOR_BLACK);
    DrawRGBImage(LogoSmall,LogoSmallXPos,LogoSmallYPos);
    //Display initial values
    SetNumber(Rpm,50,RpmXPos,RpmYPos,5,0,0);
    SetNumber(MAP,50,MAPXPos,MAPYPos,3,0,0);
    SetNumber(Air_Temp,50,AirTempXPos,AirTempYPos,3,0,0);
    SetNumber(Lambda,50,LambdaXPos,LambdaYPos,4,1,0);
    SetNumber(Volts,50,VoltsXPos,VoltsYPos,3,2,0);
    //SetNumber((int)(Meter_counter/10),50,Meter_counterXPos,Meter_counterYPos,5,3,0);
};


void SetSecondAux(){                        // Set Second Auxiliary screen
    // Erase previous screen
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0,0,800,480);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    // Draw Second Auxiliary screen
    PrintString("LVDT Sensors",50,200,20,LCD_COLOR_BLACK);
    DrawRGBImage(LogoSmall,LogoSmallXPos,LogoSmallYPos);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.SetFont(&Font24);
    lcd.DisplayStringAt(FLLVDTBarXPos+25,FLLVDTBarYPos,(uint8_t*)"FL",LEFT_MODE);
    lcd.DisplayStringAt(FRLVDTBarXPos+25,FRLVDTBarYPos,(uint8_t*)"FR",LEFT_MODE);
    lcd.DisplayStringAt(RLLVDTBarXPos+25,RLLVDTBarYPos,(uint8_t*)"BL",LEFT_MODE);
    lcd.DisplayStringAt(RRLVDTBarXPos+25,RRLVDTBarYPos,(uint8_t*)"BR",LEFT_MODE);
    // Display initial values
    SetNumber(FL_LVDT,100,FLLVDTXPos,FLLVDTYPos,3,0,1);
    SetNumber(FR_LVDT,100,FRLVDTXPos,FRLVDTYPos,3,0,1);
    SetNumber(RL_LVDT,100,RLLVDTXPos,RLLVDTYPos,3,0,1);
    SetNumber(RR_LVDT,100,RRLVDTXPos,RRLVDTYPos,3,0,1);
    UpdateLVDTScale(FL_LVDT,-100,FLLVDTBarXPos,FLLVDTBarYPos);
    UpdateLVDTScale(FR_LVDT,-100,FRLVDTBarXPos,FRLVDTBarYPos);
    UpdateLVDTScale(RL_LVDT,-100,RLLVDTBarXPos,RLLVDTBarYPos);
    UpdateLVDTScale(RR_LVDT,-100,RRLVDTBarXPos,RRLVDTBarYPos);

};
            

void MainUpdate(){                                  // Update information in Main screen
    lcd.SetTextColor(LCD_COLOR_BLACK);
    switch(ft_main_flag){                           // Check is main update is first time entered. If yes set Speed0 to 0 to acordingly draw update.
        case(0):break;
        case(1):
            Speed0=0;
            ft_main_flag=0;                         //Set flag to 0.
            break;
    };
    if(Speed!=Speed0){                              //Check is value is changed. If it is then update it on screen.
        UpdateSpeedMeter((int)(Speed0/10),(int)((Speed-Speed0)/10));
    };
    if(Gear!=Gear0){
        PrintChar(Gears[Gear],GearXPos,GearYPos,LCD_COLOR_BLACK);
    };
    if(Oil_Temp!=Oil_Temp0){
        ChangeNumber(Oil_Temp,Oil_Temp0,100,OilTempXPos,OilTempYPos,3,0,0);
    };
    if(Water_Temp!=Water_Temp0){
       ChangeNumber(Water_Temp,Water_Temp0,100,WaterTempXPos,WaterTempYPos,3,0,0);
    };
    if(TPS!=TPS0){
        ChangeNumber(TPS,TPS0,100,TPSXPos,TPSYPos,3,0,0);
    };
    if(Brakes!=Brakes0){
        BrakeSignal(Brakes);
    };
};   
    

void FirstAuxUpdate(){                      // Update information in First Auxiliary screen
    if (Rpm!=Rpm0){
        ChangeNumber(Rpm,Rpm0,50,RpmXPos,RpmYPos,5,0,0);
    };
    if (MAP!=MAP0){
        ChangeNumber(MAP,MAP0,50,MAPXPos,MAPYPos,3,0,0);
    };
    if (Air_Temp!=Air_Temp0){
        ChangeNumber(Air_Temp,Air_Temp0,50,AirTempXPos,AirTempYPos,3,0,0);
    };
    if (Lambda!=Lambda0){
        ChangeNumber(Lambda,Lambda0,50,LambdaXPos,LambdaYPos,4,1,0);
    };
    if (Volts!=Volts0){
        ChangeNumber(Volts,Volts0,50,VoltsXPos,VoltsYPos,3,2,0);
    };
    //if (Meter_counter!=Meter_counter0){
    //    ChangeNumber((int)(Meter_counter/10),(int)(Meter_counter0/10),50,Meter_counterXPos,Meter_counterYPos,5,3,0);
    //};
};

void SecondAuxUpdate(){                     // Update information in Second Auxiliary screen
    switch(ft_2aux_flag){                   //Same as in Main.
        case(0):break;
        case(1):
            FL_LVDT0=0;
            FR_LVDT0=0;
            RL_LVDT0=0;
            RR_LVDT0=0;
            ft_2aux_flag=0;
            break;
    };
    
    if(FL_LVDT!=FL_LVDT0){
        ChangeNumber(FL_LVDT,FL_LVDT0,100,FLLVDTXPos,FLLVDTYPos,3,0,1);
        UpdateLVDTScale(FL_LVDT,FL_LVDT0,FLLVDTBarXPos,FLLVDTBarYPos);
    };
    if(FR_LVDT!=FR_LVDT0){
        ChangeNumber(FR_LVDT,FR_LVDT0,100,FRLVDTXPos,FRLVDTYPos,3,0,1);
        UpdateLVDTScale(FR_LVDT,FR_LVDT0,FRLVDTBarXPos,FRLVDTBarYPos);
    };
    if(RL_LVDT!=RL_LVDT0){
        ChangeNumber(RL_LVDT,RL_LVDT0,100,RLLVDTXPos,RLLVDTYPos,3,0,1);
        UpdateLVDTScale(RL_LVDT,RL_LVDT0,RLLVDTBarXPos,RLLVDTBarYPos);
    };
    if(RR_LVDT!=RR_LVDT0){
        ChangeNumber(RR_LVDT,RR_LVDT0,100,RRLVDTXPos,RRLVDTYPos,3,0,1);
        UpdateLVDTScale(RR_LVDT,RR_LVDT0,RRLVDTBarXPos,RRLVDTBarYPos);
    };
};