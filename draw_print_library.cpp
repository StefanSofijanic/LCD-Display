#include "draw_print_library.h"
#include "gears.h"
#include "font100.h"
#include "font50.h"

extern LCD_DISCO_F469NI lcd;
extern SD_DISCO_F469NI sd;
extern Serial pc;

extern uint8_t lvdtref;
extern int FL_LVDT_Ref,FR_LVDT_Ref,RL_LVDT_Ref,RR_LVDT_Ref;

//Gears 
GEAR Idle{'0',256,384,idleBitmap};
GEAR Gear1{'1',256,384,gear1Bitmap};
GEAR Gear2{'2',256,384,gear2Bitmap};
GEAR Gear3{'3',256,384,gear3Bitmap};
GEAR Gear4{'4',256,384,gear4Bitmap};
GEAR Gear5{'5',256,384,gear5Bitmap};
GEAR Gear6{'6',256,384,gear6Bitmap};
GEAR *Gears[7]={&Idle,&Gear1,&Gear2,&Gear3,&Gear4,&Gear5,&Gear6};       //Gears array
IMAGE LogoBig{800,480,LOGOBIG_START_ADDR};                              //Big Logo 
IMAGE LogoSmall{160,64,LOGOSMALL_START_ADDR};                           //Small Logo
IMAGE Branko{250,480,1576960};                                          //Branko
IMAGE NewYearCongat{800,480,NEW_YEAR_CONGAT_START_ADDR};
//IMAGE Miljana[250,480,---};

void DrawSpeedMeter(){
    uint16_t x1,y1,x2,y2,x3,y3,x4,y4;
    double THETA1=PHI,THETA2;
    
    //Set starting point for drawing
    x1=(int)(400-530*cos(THETA1));
    y1=(int)(550-530*sin(THETA1));
    x2=(int)(400-500*cos(THETA1));
    y2=(int)(550-500*sin(THETA1));
    
    lcd.SetTextColor(LCD_COLOR_BLACK);
    for (int V=0;V<=150;V++){                   //Draw from 0 to 150
        //Set angle for next speed scale value
        THETA2=THETA1+ALPHA/Vmax;               
        x3=(int)(400-530*cos(THETA2));
        y3=(int)(550-530*sin(THETA2));
        x4=(int)(400-500*cos(THETA2));
        y4=(int)(550-500*sin(THETA2));
        if(V<150){                              //Draw arch section
            lcd.DrawLine(x1,y1,x3,y3);
            lcd.DrawLine(x2,y2,x4,y4);
        };
        lcd.SetFont(&Font20);
        if(V%10==0){                            //Print round values (0,10,20,...)
            lcd.DrawLine(x1,y1,x2,y2);          //Draw round scale value
            char Vchar[3];
            sprintf(Vchar,"%d",(int)V);
            uint16_t xnum=x2,ynum=y2;
            if (V==0 | V==10 | V==20 | V==30){  //Position of printed value
                xnum=x2,ynum=y2+5;
            }else if (V==40 | V==50 | V==60){
                xnum=x2-3,ynum=y2+5;
            }else if (V==70 | V==80){
                xnum=x2-14,ynum=y2+5;
            }else if (V==90){
                xnum=x2-20,ynum=y2+5;
            }else if (V==100 | V==110 | V==120) {
                xnum=x2-35,ynum=y2+4;
            }else if (V==130 | V==140 | V==150){
                xnum=x2-35,ynum=y2+5;
            };
            lcd.DisplayStringAt(xnum,ynum,(uint8_t*)Vchar,LEFT_MODE);
        };
        x1=x3,y1=y3,x2=x4,y2=y4;            //Increment position
        THETA1=THETA2;
    };
};

void PrintChar(CHAR Char,uint16_t StartXPos,uint16_t StartYPos,uint32_t TextColor){
    uint16_t width=Char.width;
    uint16_t height=Char.height;
    uint16_t horpos,vertpos;
    uint16_t bitloc;
    uint32_t DrawColor;
    char pos[9];
    //uint8_t dbg[50];
    for(horpos=0;horpos<width;horpos++){                                //Bitmaps are row after row
        for(vertpos=0;vertpos<height/8;vertpos++){
            sprintf(pos,BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(Char.bitmap[horpos*height/8+vertpos]));   //Convert uint8_t from hex to binary. 1 to fill, 0 to skip
            //pc.printf("%d. %s\n",horpos*height/8+vertpos,pos);
            for(bitloc=0;bitloc<8;bitloc++) {
                if (pos[bitloc]=='1') {
                    DrawColor=TextColor;
                }
                else{
                    DrawColor=lcd.GetBackColor();
                };
                lcd.DrawPixel(StartXPos+horpos,StartYPos+vertpos*8+bitloc,DrawColor);
                    //pc.printf("%d,%d\n",StartXPos+horpos,StartYPos+vertpos*8+bitloc);                   
            }
        }
    }
    lcd.SetTextColor(LCD_COLOR_BLACK);
};

void PrintString(char str[],int font,uint16_t StartXPos,uint16_t StartYPos,uint32_t TextColor){
    //pc.printf("Function activated.\n");
    char *a=str;
    int p=0;
    while(a[p]){
        //pc.printf("Searching char: %c\n",a[p]);
        for(int k=0;k<68;k++){
            //pc.printf("Character %c\n",(*font50[k]).name);
            if((*font50[k]).name==a[p]){
          //      pc.printf("Found!\n");
                PrintChar(*font50[k],StartXPos,StartYPos,TextColor);
                StartXPos+=(*font50[k]).width;
                k=100;
            };
        };
        //pc.printf("End.\n");
        p++;
    };
};
                    
                                        


void ChangeNumber(int num,int num0,int Font,uint16_t StartXPos,uint16_t StartYPos, int digits, int dec_point, int sign){
    //Function only changes the decimals that change. If number changes from 146 to 147, it only changes 6 to 7.
    int digit[digits],digit0[digits];
    int sum=0,sum0=0,k,aux=0;
    CHAR **font;
    if(abs(num)<pow(float(10),digits)){
        digit[0]=abs(num)/pow((float)10,(float)(digits-1));
        digit0[0]=abs(num0)/pow((float)10,(float)(digits-1));
        for(k=1;k<digits;k++){
            sum=(sum+digit[k-1])*10;
            digit[k]=abs(num)/pow((float)10,(float)(digits-1-k))-sum;
            sum0=(sum0+digit0[k-1])*10;
            digit0[k]=abs(num0)/pow(10,(float)(digits-1-k))-sum0;
        };
    
        if (Font==50){
            font=font50;
        }else if (Font==100){
            font=font100;
        };
    
        lcd.SetTextColor(LCD_COLOR_BLACK);
        uint8_t char_width=(*font[0]).width;
        
        if (sign==1){
            if(num<0 & num0>=0){                                                //Print minus if number lower than 0
                PrintChar(*font[10],StartXPos,StartYPos,lcd.GetTextColor());
            }else if(num>=0 & num0<0){                                          //Print blank if number lower than 0 
                lcd.SetTextColor(LCD_COLOR_WHITE);
                lcd.FillRect(StartXPos,StartYPos,(*font[10]).width,(*font[10]).height);
                lcd.SetTextColor(LCD_COLOR_BLACK);
            };
            aux+=(*font[10]).width;
        };
           
        for(k=0;k<digits;k++){
            if(dec_point==k & dec_point>0){  //Default is for font50. This is not an universal solution, but it is in use because all font 100 infos are integers
                PrintChar(Chardot_50,StartXPos+dec_point*char_width,StartYPos,lcd.GetTextColor());
                aux+=Chardot_50.width;
            };
            if(digit[k]!=digit0[k]){
                PrintChar(*font[digit[k]],StartXPos+k*char_width+aux,StartYPos,lcd.GetTextColor());
            };
        };   
    };
};

void SetNumber(int num,int Font,uint16_t StartXPos,uint16_t StartYPos, int digits, int dec_point, int sign){
    int digit[3],sum=0,k,aux=0;
    CHAR **font;
    
    if (Font==50){
        font=font50;
    }else if (Font==100){
        font=font100;
    };
    
    lcd.SetTextColor(LCD_COLOR_BLACK);
    uint8_t char_width=(*font[0]).width;
    
    if(num<pow(float(10),digits)){
        digit[0]=abs(num)/pow((float)10,(float)(digits-1));
        for(k=1;k<digits;k++){
            sum=(sum+digit[k-1])*10;
            digit[k]=abs(num)/pow((float)10,(float)(digits-1-k))-sum;
        };
        
        if(sign==1){
            if(num<0){                                                          //Print minus if number lower than 0
                PrintChar(*font[10],StartXPos,StartYPos,lcd.GetTextColor());
            }else if(num>=0){                                                   //Print blank if number lower than 0 
                lcd.SetTextColor(LCD_COLOR_WHITE);
                lcd.FillRect(StartXPos,StartYPos,(*font[10]).width,(*font[10]).height);
                lcd.SetTextColor(LCD_COLOR_BLACK);
            };
            aux+=(*font[10]).width;
        };
              
        for(k=0;k<digits;k++){
            if(dec_point==k & dec_point>0){  //Default is for font50. This is not an universal solution, but it is in use because all font 100 infos are integers
                PrintChar(Chardot_50,StartXPos+dec_point*char_width,StartYPos,lcd.GetTextColor());
                aux+=Chardot_50.width;
            };
            PrintChar(*font[digit[k]],StartXPos+k*char_width+aux,StartYPos,lcd.GetTextColor());
        };
    };   
};   

void DrawRGBImage(IMAGE Image,uint16_t StartXPos,uint16_t StartYPos){
    uint32_t p,q;
    uint32_t BlockBuffer[128];
    uint16_t xpos=0,ypos=0;
    for(p=0;p<Image.width*Image.height/128;p++){
        sd.ReadBlocks(BlockBuffer,Image.START_ADDR+p*512,1,SD_DATATIMEOUT);
        for(q=0;q<128;q++){
            if(ypos<Image.height){
               lcd.DrawPixel(StartXPos+xpos,StartYPos+ypos,BlockBuffer[q]);
            }
            else{
                xpos++;
                ypos=0;
                lcd.DrawPixel(StartXPos+xpos,StartYPos+ypos,BlockBuffer[q]);
            }
            ypos++;
        };
    };
};  


void UpdateSpeedMeter(int V, int dV){
   //Prednost koda je sto se docrtava samo onaj deo koji se menja. Tako ako sa 55 kmh prelazimo na 57 khm on obradjuje samo polja 55 i 56.
   //Kada bi se samo brisala stara i crtala nova vrednost na baru ukupno bi morali da obradimo 55+57=112 umesto samo 2 bara.
   uint32_t OldColor,RewriteColor;
   double THETA1,THETA2;
   uint16_t x1,y1,x2,y2,x3,y3,x4,y4;
   uint16_t startx,starty,leftupx,rightupx,leftdownx,rightdownx,ypos;

    //Obradjivanje polje po polje. Svako polje prestavlja odredjenu vrednost kmh.  
    for(int k=1;k<=abs(dV);k++){
        //Uokviravanje polja koje se trenutno obradjuje. Granice su crne kako bi se jasno razgranicilo polje od ostatka bara.
        lcd.SetTextColor(LCD_COLOR_BLACK);
        THETA1=PHI+((double)V)*ALPHA/Vmax;
        x1=(int)(400-530*cos(THETA1));
        y1=(int)(550-530*sin(THETA1));
        x2=(int)(400-500*cos(THETA1));
        y2=(int)(550-500*sin(THETA1));
        THETA2=PHI+((double)(V+dV/abs(dV)))*ALPHA/Vmax;
        x3=(int)(400-530*cos(THETA2));
        y3=(int)(550-530*sin(THETA2));
        x4=(int)(400-500*cos(THETA2));
        y4=(int)(550-500*sin(THETA2));
        lcd.DrawLine(x1,y1,x2,y2);
        lcd.DrawLine(x3,y3,x4,y4);
        lcd.DrawLine(x1,y1,x3,y3);
        lcd.DrawLine(x2,y2,x4,y4);
        
        //Odredjivanje pocetnih koordinata i kojom bojom ce se polje bojiti.
        if(dV>0){
            startx=(x2+x3)/2;
            starty=(y2+y3)/2;
            if(V+1<=50){
                RewriteColor=LCD_COLOR_GREEN;
            }else if (V+1>50 & V+1<=100){
                RewriteColor=LCD_COLOR_BLUE;
            }else{
                RewriteColor=LCD_COLOR_RED;
            };
            OldColor=LCD_COLOR_WHITE;
        }else{
            startx=(x1+x4)/2;
            starty=(y1+y4)/2;
            if(V<=50){
                OldColor=LCD_COLOR_GREEN;
            }else if (V>50 & V<=100){
                OldColor=LCD_COLOR_BLUE;
            }else{
                OldColor=LCD_COLOR_RED;
            };
            RewriteColor=LCD_COLOR_WHITE;
        };
        lcd.SetTextColor(RewriteColor);
        
        //Odredjivanje pocetnog piksela odakle ce bojenje poceti. Kako bi se svaki piksel obojio pocetni se postavlja u centru polja i krece se najpre
        //na gore pa zatim od sredine na dole.
        leftupx=startx;
        rightupx=startx+1;
        ypos=starty; 
        while(leftupx<rightupx){
            
            ypos--;
            if(lcd.ReadPixel(leftupx,ypos)==LCD_COLOR_BLACK){
                while(lcd.ReadPixel(leftupx,ypos)==LCD_COLOR_BLACK){
                    leftupx++;
                }
            } else {
                while(lcd.ReadPixel(leftupx,ypos)==OldColor){
                    leftupx--;
                };
                leftupx++;
            };
            if(lcd.ReadPixel(rightupx,ypos)==LCD_COLOR_BLACK){
                while(lcd.ReadPixel(rightupx,ypos)==LCD_COLOR_BLACK){
                    rightupx--;
                }
            } else {
                while(lcd.ReadPixel(rightupx,ypos)==OldColor){
                    rightupx++;
                };
                rightupx--;
            };
            
            if (leftupx<=rightupx){
                lcd.DrawLine(leftupx,ypos,rightupx,ypos);
            };   
        };
        
        leftdownx=startx;
        rightdownx=startx+1;
        ypos=starty-1; 
        while(leftdownx<rightdownx){
            ypos++;
            if(lcd.ReadPixel(leftdownx,ypos)==LCD_COLOR_BLACK){
                while(lcd.ReadPixel(leftdownx,ypos)==LCD_COLOR_BLACK){
                    leftdownx++;
                }
            } else {
                while(lcd.ReadPixel(leftdownx,ypos)==OldColor){
                    leftdownx--;
                };
                leftdownx++;
            };
            if(lcd.ReadPixel(rightdownx,ypos)==LCD_COLOR_BLACK){
                while(lcd.ReadPixel(rightdownx,ypos)==LCD_COLOR_BLACK){
                    rightdownx--;
                }
            } else {
                while(lcd.ReadPixel(rightdownx,ypos)==OldColor){
                    rightdownx++;
                };
                rightdownx--;
            };
            
            if (leftdownx<=rightdownx){
                lcd.DrawLine(leftdownx,ypos,rightdownx,ypos);
            };   
        };
        
        //Brisanje granicnika. Vodi se racuna ako je granica neki od dekadnih podeoka koji treba da ostane crn.
        if(V%10!=0){
            lcd.DrawLine(x1,y1,x2,y2);
            if (dV<0){
                lcd.DrawPixel(x1,y1,LCD_COLOR_BLACK);
                lcd.DrawPixel(x2,y2,LCD_COLOR_BLACK);
            };
        };
        V+=dV/abs(dV);
    };
};

void ChangeCrank(int Crank){
    PrintString("     ",50,350,400,LCD_COLOR_GREEN);
    if (Crank){
        PrintString("ERROR",50,CrankXPos,CrankYPos,LCD_COLOR_RED);
        lcd.SetTextColor(LCD_COLOR_BLACK);
    } else {
        PrintString("OK",50,CrankXPos,CrankYPos,LCD_COLOR_GREEN);
        lcd.SetTextColor(LCD_COLOR_BLACK);
    };  
};

int UpdateLVDTScale(int H,int H0, uint16_t StartXPos, uint16_t StartYPos){
    uint16_t BarWidth=80,BarHeight=8;
    int YPos;
    int D,D0,dD;

    switch( ((H>=0)<<1)+(H0>0)){
        case(0):
            D=5-abs(H)/20;
            D0=5-abs(H0)/20;
            break;
        case(1):
            D=5-abs(H)/20;
            D0=H0/20+5;
            break;
        case(2):
            D=H/20+5;
            D0=5-abs(H0)/20;
            break;             
        case(3):
            D=H/20+5;
            D0=H0/20+5;
            break;
    };
    dD=D-D0;
    
    if (dD>0){
        lcd.SetTextColor(LCD_COLOR_BLACK);
        YPos=StartYPos-10*(D0+1);
    }else{
        lcd.SetTextColor(LCD_COLOR_WHITE);
        YPos=StartYPos-D0*10;
    };
    for (int k=1;k<=abs(dD);k++){
        lcd.FillRect(StartXPos,YPos,BarWidth,BarHeight);
        YPos-=10*(dD/abs(dD));
    };
    lcd.SetTextColor(LCD_COLOR_BLACK);
    return 1;
};
    

void BrakeSignal(uint16_t brake){
    if (brake!=0){
        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.SetBackColor(LCD_COLOR_RED);
        lcd.FillRect(560,340,230,68);
        PrintString("BRAKE",50,575,350,LCD_COLOR_BLACK);
    }else {
        lcd.SetTextColor(LCD_COLOR_DARKRED);
        lcd.SetBackColor(LCD_COLOR_DARKRED);
        lcd.FillRect(560,340,230,68);
        PrintString("BRAKE",50,575,350,LCD_COLOR_BLACK);
    };
    lcd.SetBackColor(LCD_COLOR_WHITE);
};
        

void TestFont(){
    uint16_t XPos=0,YPos=120;
    for(int k=0;k<68;k++){
        PrintChar(*font50[k],XPos,YPos,lcd.GetTextColor());
        XPos=XPos+(*font50[k]).width;
        wait(0.1);
        //pc.printf("%d",k);
    };
};