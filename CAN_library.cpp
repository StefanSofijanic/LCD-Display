#include "CAN_library.h"

extern CAN can;
extern Serial pc;
extern SD_DISCO_F469NI sd;
//extern EEPROM_DISCO_F469NI eep;
extern Timer t_int,t_store;
extern CANMessage msgDTA1, msgDTA2, msgDTA3, msgDTA4, msgDTA5, msgDTA6, msgLVDTFront, msgLVDTRear, msgBrakes, msgDistance;
extern uint16_t rx_flag;
extern uint8_t distance_flag;
extern uint8_t lvdtref;

extern uint16_t Rpm0, Speed0, Gear0, Water_Temp0, Oil_Temp0, TPS0, Brakes0, MAP0, Air_Temp0, Lambda0, Volts0;
extern uint16_t Rpm,  Speed,  Gear,  Water_Temp,  Oil_Temp,  TPS,  Brakes,  MAP,  Air_Temp,  Lambda,  Volts;
extern int FL_LVDT0, FR_LVDT0, RL_LVDT0, RR_LVDT0, FL_LVDT,  FR_LVDT,  RL_LVDT,  RR_LVDT;
extern int FL_LVDT_Ref,FR_LVDT_Ref,RL_LVDT_Ref,RR_LVDT_Ref;
int LVDT_Max=1024;
//extern float Meter_counter,Meter_counter0;
extern char cmd[3];
extern int addr;
extern I2C i2c;

extern void showLedRpm(int rpm);


void CANMsgReceive(){                     // CAN RX Interrupt Function
    CANMessage tmpMsg;
    if (can.read(tmpMsg)) {                 //Detect message
        //pc.printf("%x\n",tmpMsg.id);       
        switch(tmpMsg.id){                  //Find which DTA message is received 
            case(0x2000):
                rx_flag|=(1<<0);
                msgDTA1=tmpMsg;
                //pc.printf("%x\n", tmpMsg.id);
                break;
            case(0x2001):
                rx_flag|=(1<<1);
                msgDTA2=tmpMsg;
                //pc.printf("rx_flag=%x\n",rx_flag);
                break;
            case(0x2002):
                rx_flag|=(1<<2);
                msgDTA3=tmpMsg;
                break;
            case(0x2003):
                rx_flag|=(1<<3);
                msgDTA4=tmpMsg;
                break;
            case(0x2004):
                rx_flag|=(1<<4);
                msgDTA5=tmpMsg;
                break;
            case(0x2005):
                rx_flag|=(1<<5);
                msgDTA6=tmpMsg;
                break;
            case(0x1006):
                rx_flag|=(1<<6);
                msgLVDTFront=tmpMsg;
                break;
            case(0x1007):
                rx_flag|=(1<<7);
                msgLVDTRear=tmpMsg;
                break;
            case(0x1008):
                rx_flag|=(1<<8);
                msgBrakes=tmpMsg;
                break;
            case(0x1009):
                rx_flag|=(1<<9);
                msgDistance=tmpMsg;
        };
        //pc.printf("rx_flag=%d\n",rx_flag);
    };
};

void UpdateInfo(){                          // Update info for received values
    int noID=10;
    uint16_t *newData;
    for (int i=0;i<noID;i++){               //For loop goes trough Message IDs
        uint16_t flagBit;
        flagBit = rx_flag & (1<<i);         //Set flag bit for adequate message. If there is a message waiting to be stored, flagBit will be different than 0.
        if (flagBit) {                      // =/=0, there is a message, =0, there is no message                      
            switch(i){
                case(0):
                    newData=AllocData(msgDTA1);     //Allocate message to a receiver
                    UpdateInfoDTA1(newData);        //Update variables
                    rx_flag = rx_flag ^ (1<<0);     //Set flag of adequate bit to 0
                    break;
                case(1):
                    newData=AllocData(msgDTA2);
                    UpdateInfoDTA2(newData);
                    rx_flag = rx_flag ^ (1<<1);
                    break;
                case(2):
                    newData=AllocData(msgDTA3);
                    UpdateInfoDTA3(newData);
                    rx_flag = rx_flag ^ (1<<2);
                    break;
                case(3):
                    newData=AllocData(msgDTA4);
                    UpdateInfoDTA4(newData);
                    rx_flag = rx_flag ^ (1<<3);
                    break;
                case(4):
                    newData=AllocData(msgDTA5);
                    UpdateInfoDTA5(newData);
                    rx_flag = rx_flag ^ (1<<4);
                    break;
                case(5):
                    newData=AllocData(msgDTA6);
                    UpdateInfoDTA6(newData);
                    rx_flag = rx_flag ^ (1<<5);
                    break;
                case(6):
                    newData=AllocData(msgLVDTFront);
                    UpdateInfoLVDTFront(newData);
                    rx_flag = rx_flag ^ (1<<6);
                    break;
                case(7):
                    newData=AllocData(msgLVDTRear);
                    UpdateInfoLVDTRear(newData);
                    rx_flag = rx_flag ^ (1<<7);
                    break;
                case(8):
                    newData=AllocData(msgBrakes);
                    UpdateInfoBrakes(newData);
                    rx_flag = rx_flag ^ (1<<8);
                    break;
                case(9):
                    newData=AllocData(msgDistance);
                    //if(distance_flag==0){
                    //    InitDistanceMeter(newData);
                    //    distance_flag=1;
                    //};
                    rx_flag = rx_flag ^ (1<<9);
                // for each new id add new case statement
            };
        };
    };
    //UpdateMeterCounter();
};

uint16_t * AllocData(CANMessage msg){                   //Unpack CAN message 
    uint16_t newData[4];
    newData[0]=(msg.data[1]<<8) | msg.data[0];
    newData[1]=(msg.data[3]<<8) | msg.data[2];
    newData[2]=(msg.data[5]<<8) | msg.data[4];
    newData[3]=(msg.data[7]<<8) | msg.data[6];
    return newData;
};

/*void InitDistanceMeter(uint16_t newData[4]){
    uint32_t DistanceBuffer;
    DistanceBuffer=(uint32_t)((newData[1]<<16) | (newData[0]));
    Meter_counter=float(DistanceBuffer)*10;
    //pc.printf("Init success, start km=%d,%f\n",DistanceBuffer,Meter_counter);
    t_int.start();
    t_store.start();
};*/


void UpdateInfoDTA1(uint16_t newData[4]){                //Update values for DTA1 message
    if (newData[0]>=0 && newData[0]<=13000){
        Rpm0=Rpm;
        Rpm=newData[0];
        showLedRpm(Rpm);
    };
    if (newData[1]>0 && newData[1]<100){
        TPS0=TPS;
        TPS=newData[1];
    };
    if (newData[2]>0 && newData[2]<1000){
        Water_Temp0=Water_Temp;
        Water_Temp=newData[2];
    };
    if (newData[3]>0 && newData[3]<1000){
        Air_Temp0=Air_Temp;
        Air_Temp=newData[3];
    };
};

void UpdateInfoDTA2(uint16_t newData[4]){                //Update values for DTA2 message
    if (newData[0]>0 && newData[0]<200){
        MAP0=MAP;
        MAP=newData[0];
    };
    if (newData[1]>0 && newData[2]<10000){
        Lambda0=Lambda;
        Lambda=newData[1];
    };
    if (newData[2]>=0 && newData[2]<1500){
        if (abs(Speed-Speed0)>10 ){
            Speed0=Speed;
        };
        Speed=newData[2];
    };
};

void UpdateInfoDTA3(uint16_t newData[4]){                //Update values for DTA3 message
    if (newData[1]>0 && newData[1]<200){
        Oil_Temp0=Oil_Temp;
        Oil_Temp=newData[1];
    };
    if (newData[2]>0 && newData[2]<200){
        Volts0=Volts;
        Volts=newData[2];
    };
};

void UpdateInfoDTA4(uint16_t newData[4]){                //Update values for DTA4 message
    if (newData[0]>=0 && newData[0]<=6){
        Gear0=Gear;
        Gear=newData[0];
    };
};

void UpdateInfoDTA5(uint16_t newData[4]){                //Update values for DTA5 message
};

void UpdateInfoDTA6(uint16_t newData[4]){                //Update values for DTA6 message
};

/*void UpdateMeterCounter(){
    float ti=t_int.read();
    if(ti>0.1){
        Meter_counter0=Meter_counter;
        Meter_counter=Meter_counter+(float)(Speed*0.0277778);    // 0.1*1000/3600
        t_int.reset();
    };
    if(t_store>2){
        char DistanceBuffer8[8];
        uint32_t DistanceBuffer32=uint32_t(Meter_counter);
        DistanceBuffer32=uint32_t(Meter_counter);
        DistanceBuffer8[0]=uint8_t(DistanceBuffer32>>24);
        DistanceBuffer8[1]=uint8_t((DistanceBuffer32 & (uint32_t)0x00FF0000)>>16);
        DistanceBuffer8[2]=uint8_t((DistanceBuffer32 & (uint32_t)0x0000FF00)>>8);
        DistanceBuffer8[3]=uint8_t((DistanceBuffer32 & (uint32_t)0x000000FF));
        CANMessage msg;
        if(can.write(CANMessage((uint16_t)DISTANCE,DistanceBuffer8, 8))) {
            pc.printf("%d\n",DistanceBuffer32);
        };
        t_store.reset();
    
        uint32_t distance_buffer;
        distance_buffer=uint32_t(Meter_counter) ;
        pc.printf("distance=%d\n",distance_buffer);
        distance_buffer=0xFF000000;
        if(sd.WriteBlocks((uint32_t*)distance_buffer,DISTANCE_MEMORY_ADDR,1,SD_DATATIMEOUT)){
            pc.printf("Storage success\n");
        }else{
            pc.printf("Storage failed\n");
        };
        
        uint32_t DistanceBuffer32;
        uint8_t DistanceBuffer8[4];
        DistanceBuffer32=uint32_t(Meter_counter);
        DistanceBuffer8[0]=DistanceBuffer32>>24;
        DistanceBuffer8[1]=(DistanceBuffer32 & (uint32_t)0x00FF0000)>>16;
        DistanceBuffer8[2]=(DistanceBuffer32 & (uint32_t)0x0000FF00)>>8;
        DistanceBuffer8[3]=(DistanceBuffer32 & (uint32_t)0x000000FF);
        if(eep.WriteBuffer(DistanceBuffer8,0,4)){
            pc.printf("Write success,Distance=%d\n",DistanceBuffer32);
        };
    };
};   

void send(uint16_t ID, char dta[8]) {
    if(can.write(CANMessage((uint16_t)ID,dta, 8))) {
        printf("%x\n",ID);
        printf("Message sent: info1=%x%x, info2=%x%x, info3=%x%x, info4=%x%x\n", dta[1],dta[0],dta[3],dta[2],dta[5],dta[4],dta[7],dta[6]);
    }else{
        printf("Shit\n");
    } 
};*/

void UpdateInfoLVDTFront(uint16_t newData[4]){           //Update values for LVDTFront message
    if (newData[0]>0 && newData[0]<=1024){
        FL_LVDT0=FL_LVDT;                                    //LVDTs are firstly written their voltage value, then overwritten by calculated pertentage value.
        FL_LVDT=newData[0];
        if(!(lvdtref & (1<<3))){                             //Check if its the first time LVDT value is received. If yes than store it as Referrent value.
            if (FL_LVDT>FL_LVDT_Ref){
                FL_LVDT=(FL_LVDT-FL_LVDT_Ref)*100/(LVDT_Max-FL_LVDT_Ref);
            }else{
                FL_LVDT=(FL_LVDT-FL_LVDT_Ref)*100/FL_LVDT_Ref;
            };
        }else{    
            FL_LVDT_Ref=FL_LVDT;
            FL_LVDT=0;
            lvdtref=lvdtref^(1<<3);
        };
    };
    
    if (newData[1]>0 && newData[1]<=1024){
        FR_LVDT0=FR_LVDT;
        FR_LVDT=newData[1];    
        if(!(lvdtref & (1<<2))){
                if (FR_LVDT>FR_LVDT_Ref){
            FR_LVDT=(FR_LVDT-FR_LVDT_Ref)*100/(LVDT_Max-FR_LVDT_Ref);
            }else{
                FR_LVDT=(FR_LVDT-FR_LVDT_Ref)*100/FR_LVDT_Ref;
            };
        }else{    
            FR_LVDT_Ref=FR_LVDT;
            FR_LVDT=0;
            lvdtref=lvdtref^(1<<2);
        };
    };
};

void UpdateInfoLVDTRear(uint16_t newData[4]){            //Update values for LVDTRear message
    if (newData[0]>0 && newData[0]<=1024){
        RL_LVDT0=RL_LVDT;
        RL_LVDT=newData[0];
        if(!(lvdtref & (1<<1))){
            if (RL_LVDT>RL_LVDT_Ref){
                RL_LVDT=(RL_LVDT-RL_LVDT_Ref)*100/(LVDT_Max-RL_LVDT_Ref);
            }else{
                RL_LVDT=(RL_LVDT-RL_LVDT_Ref)*100/RL_LVDT_Ref;
            };
        }else{    
            RL_LVDT_Ref=RL_LVDT;
            RL_LVDT=0;
            lvdtref=lvdtref^(1<<1);
        };
    };   
    
    if (newData[1]>0 && newData[1]<=1024){
        RR_LVDT0=RR_LVDT;
        RR_LVDT=newData[1];    
        if(!(lvdtref & (1<<0))){
            if (RR_LVDT>RR_LVDT_Ref){
                RR_LVDT=(RR_LVDT-RR_LVDT_Ref)*100/(LVDT_Max-RR_LVDT_Ref);
            }else{
                RR_LVDT=(RR_LVDT-RR_LVDT_Ref)*100/RR_LVDT_Ref;
            };
        }else{    
            RR_LVDT_Ref=RR_LVDT;
            RR_LVDT=0;
            lvdtref=lvdtref^(1<<0);
        };
    };
};

void UpdateInfoBrakes(uint16_t newData[4]){              //Update values for Brakes message
    //if (newData[2]==0xFF || newData[2]==0x00){
        Brakes0=Brakes;
        Brakes=newData[2];
        //pc.printf("%x\n",Brakes);
    //      };
};