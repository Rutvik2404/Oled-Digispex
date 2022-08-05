
#include <string.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;

uint32_t tsLastReport = 0;
int current_heart,sop2;
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(A1, A2); // RX, TX


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     0// Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

int a,b,c,d,current_time=0,alarm_time;
int time_out=0;

String inputData = "";        
bool inputDataOver = false;
String SM_ARU="";


char  SM_ARU_0_DA[50];

//String SM_ARU_0_DA="";
String SM_ARU_1_BT="";
String SM_ARU_2_ME="";
String SM_ARU_3_CA="";
String SM_ARU_4_NE="";
String SM_ARU_5_AL="";

const int Buzzer=11;

int alarm_data=0;
int clock_data=0;
int battery_data=0;
int message_data=0;
int call_data=0;
int net_data=0;

const int SW_Press = 12;  
int Sw_State=0;

int Battery_level=0;
int count_b=0;
int valid_data=0;

int m=0;
  
void onBeatDetected()
{
     Serial.println("Pluse");
   
      display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);            
  display.println("Pluse....");
  display.display();
}
  
void setup()
{
  
    Serial.begin(9600);
    bluetooth.begin(9600);   
    Serial.println("SM_ARU...");
    bluetooth.println("SM_ARU...");
    
    pinMode(SW_Press, INPUT);
    digitalWrite(SW_Press, HIGH);
    
    pinMode(Buzzer, OUTPUT);
    digitalWrite(Buzzer, HIGH);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("Smart "));
    display.setCursor(8,17);             // Start at top-left corner
    display.println(F(" Glass"));
    display.display();
    delay(1000); 
  
    

    
  //init_sensor();
  count_b=30000;
  display.clearDisplay();  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.display();
  
    Battery_level=analogRead(A0); 
  
}

void loop() 
{ 
  if(count_b>=50000)
  {
    Battery_level=analogRead(A0); 
    Serial.print("Battery_level = ");
    //Serial.println(Battery_level);
    //delay(100);
    count_b=0; 
  }
  delay(1);
  count_b++;
if(Battery_level>500)
{
  read_data(); 
  
  Sw_State = digitalRead(SW_Press);
  if (Sw_State == 0)
  {
     time_out=0;
     init_sensor();
     read_Sensor();
     delay(2000);
  }//  delay(100);
  time_out++;
  if(time_out==50000)
  {
      Serial.println("Time Out");
      display.clearDisplay();
      
      display.setTextSize(1);             
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(7,10);            
      display.println(F("System Disconnect"));
      
      display.display();
  }
}
  else
  {
      Serial.println("Battery Low");
      display.clearDisplay();    
      display.setTextSize(1);             
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(7,10);            
      display.println(F("System Battery Low"));
      digitalWrite(Buzzer, 0);delay(200);    
      digitalWrite(Buzzer, 1);   
      display.display();  
  }

}


void init_sensor()
{
  display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);            
  display.println("Initializing...");
  display.setCursor(4,20);            
  display.println("Pulse Oximeter...");
  display.display();
    if (!pox.begin())
    {
  //  Serial.print("FAILED");
        for(;;);
    } else
      {
       // Serial.println("SUCCESS");
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
}



void read_spo2()
{
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate: ");
        Serial.print(pox.getHeartRate());
        current_heart=(pox.getHeartRate());
        Serial.print("  bpm / SpO2: ");
        Serial.println(pox.getSpO2());
        sop2=(pox.getSpO2());
      //  Serial.println("%");

        tsLastReport = millis();
   }
   if(sop2<70)sop2=92;
}

void read_Sensor()
{
  int i=0;
    init_sensor(); 
    read_spo2();
    current_heart=0;i=0;
  display.setCursor(7,20);            
  display.println(".");
  display.display();
    while(i<=1)
    {
      Serial.print('.'); 
      
   // current_heart=0;i=0;
      read_spo2();
      if( current_heart>=50)i++;
      delay(50);
    }     
  display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);            
  display.print("HeartBeat: ");
  display.print(current_heart);
  display.print("bpm");
  display.setCursor(7,20);            
  display.print("SPo2: ");
  display.print(sop2);
  display.print("%");
  display.display();

  
}
void read_data() 
{
  while (bluetooth.available())
  {
    char inChar = (char)bluetooth.read();
    
    if(inChar == '*')valid_data=1;
    if(valid_data==1)SM_ARU += inChar;
    
    if (inChar == '#') 
    {
      valid_data=0;
      inputDataOver = true;
      SM_ARU+='\n';
      inChar='0';
    }
  }
   if (inputDataOver) 
  { 
      time_out=0;
      Serial.println(SM_ARU);
      if(SM_ARU[0]=='*' && SM_ARU[1]=='0' && SM_ARU[27]=='#')
      {
      for(int v=0;v<=50;v++)
        SM_ARU_0_DA[v]=0;
m=0;
        while(SM_ARU[m]!='#')
      {SM_ARU_0_DA[m]=SM_ARU[m];m++;}
      
     // SM_ARU_0_DA=SM_ARU;
      Serial.println(SM_ARU_0_DA);clock_data=1; Display_1_Data_1();}
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='1')
      {SM_ARU_1_BT = "";SM_ARU_1_BT=SM_ARU;Serial.println(SM_ARU_1_BT);battery_data=1;Display_1_Data_1(); }
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='2')
      {SM_ARU_2_ME = "";SM_ARU_2_ME=SM_ARU;Serial.println(SM_ARU_2_ME);message_data=1;if(SM_ARU_2_ME[3]=='P')Display_2(); }
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='3')
      {SM_ARU_3_CA = "";SM_ARU_3_CA=SM_ARU;Serial.println(SM_ARU_3_CA);call_data=1;if(SM_ARU_3_CA[3]=='P')Display_3(); }
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='4')
      {SM_ARU_4_NE = "";SM_ARU_4_NE=SM_ARU;Serial.println(SM_ARU_4_NE);net_data=1;Display_1_Data_1(); }
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='5')
      {SM_ARU_5_AL = "";SM_ARU_5_AL=SM_ARU;Serial.println(SM_ARU_5_AL);alarm_data=1;Display_1_Data_1(); }
      
      if(SM_ARU[0]=='*' && SM_ARU[1]=='6' && SM_ARU[3]=='#')
      {Serial.println("End");Display_1_Data_1();    
      alarm_data=0;clock_data=0;battery_data=0;
      message_data=0;call_data=0;net_data=0;}
      
      //Data_process(); 
      inputDataOver = false;  
      SM_ARU = "";
  }
}

void Data_process()
{
  
// 0 -> Date & Time
// 1 -> Battery
// 2 -> Message
// 3 -> Call
// 4 -> Net
// 5 -> Alarm

    Serial.println(SM_ARU); 

 
    Serial.print(SM_ARU[0]);
    Serial.print(SM_ARU[1]);
    Serial.println(SM_ARU[27]);  
    //Serial.println(SM_ARU[27]);  

    if(SM_ARU[0]=='*' && SM_ARU[1]=='0' && SM_ARU[27]=='#')
    {
      Serial.println("Date & Time");
      clock_data=1; 

      for(int v=0;v<=50;v++)
        SM_ARU_0_DA[v]=0;
m=0;
        while(SM_ARU[m]!='#')
      {SM_ARU_0_DA[m]=SM_ARU[m];m++;}
      
      Serial.println(SM_ARU_0_DA);
      Display_1_Data_1();
    }  
//    
    if(SM_ARU[0]=='*' && SM_ARU[1]=='1')
    {
      Serial.println("Battery"); 
      battery_data=1;
      SM_ARU_1_BT=SM_ARU;
      Serial.println(SM_ARU_1_BT);
      //Display_1_Data_1();
    }  
//    
    if(SM_ARU[0]=='*' && SM_ARU[1]=='2')
    {
      Serial.println("Message");
      
      SM_ARU_2_ME = "";
      SM_ARU_2_ME=SM_ARU; 
      
      Serial.println(SM_ARU_2_ME);
      
      Serial.println(SM_ARU_2_ME[3]);          
      if(SM_ARU_2_ME[3]=='P')Display_3();
    }    
 
    if(SM_ARU[0]=='*' && SM_ARU[1]=='3')
    {
      Serial.println("Call");
      SM_ARU_3_CA = "";
      SM_ARU_3_CA=SM_ARU; 
      
      Serial.println(SM_ARU_3_CA);
      
      Serial.println(SM_ARU_3_CA[3]);          
      if(SM_ARU_3_CA[3]=='P')Display_2();
    }  
//    
   if(SM_ARU[0]=='*' && SM_ARU[1]=='4')
    {
      Serial.println("Internet");
      net_data=1;
      SM_ARU_4_NE = "";
      SM_ARU_4_NE=SM_ARU;
      
      Serial.println(SM_ARU_4_NE);
      //Display_1_Data_1();
    }
//    
    if(SM_ARU[0]=='*' && SM_ARU[1]=='5' && SM_ARU[2]=='=')
    {
      Serial.println("Alarm");
      alarm_data=1;
      SM_ARU_5_AL = "";
      SM_ARU_5_AL=SM_ARU;
      
      Serial.println(SM_ARU_5_AL);
      //Display_1_Data_1();
    }  
  if(SM_ARU[0]=='*' && SM_ARU[1]=='6' && SM_ARU[3]=='#')
    {
      Serial.println("Final Data");
      Display_1_Data_1();
      alarm_data=0;
      clock_data=0;
      battery_data=0;
      message_data=0;
      call_data=0;
      net_data=0;
      delay(1000);
    }
//    
   delay(1); 
}


void Display_1_Data_1()// Date Time Battert Net Alarm
{
 // Serial.println("LCD Update ");
  display.clearDisplay();  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);


   if(alarm_data==0
      && clock_data==0
      && battery_data==0
      && message_data==0
      && call_data==0
      && net_data==0)
      {
           display.setCursor(20,10);            
           display.print("No Input Data"); 
      }
      
  if(clock_data==1)
  {
  display.setCursor(0,2);
  for(int i=3;i<=5;i++)            
  display.print(SM_ARU_0_DA[i]);// Date & Time

  display.print(" ");
  
  for(int i=16;i<=17;i++)            
  display.print(SM_ARU_0_DA[i]);//Day
  display.print('-');
  for(int i=13;i<=14;i++)            
  display.print(SM_ARU_0_DA[i]);//Month
  display.print('-'); 
  for(int i=10;i<=11;i++)            
  display.print(SM_ARU_0_DA[i]);//year
  
  display.print(" ");
  for(int i=19;i<=20;i++)            
  display.print(SM_ARU_0_DA[i]);//HH 
  display.print(':');  
  for(int i=22;i<=23;i++)            
  display.print(SM_ARU_0_DA[i]);//MM

  a=SM_ARU_0_DA[19]-0x30;
  b=SM_ARU_0_DA[20]-0x30;
  c=SM_ARU_0_DA[22]-0x30;
  d=SM_ARU_0_DA[23]-0x30;
  
  current_time=((a*1000)+(b*100)+(c*10)+d);
//  Serial.print("current_time");
//  Serial.println(current_time);
  }  

    if(net_data==1)
    {
  display.setCursor(115,2);
//  
//  Serial.println(SM_ARU_4_NE[3]);          
  if(SM_ARU_4_NE[3]=='C')display.print("E");// Internet
  if(SM_ARU_4_NE[3]=='D')display.print("-");// Internet
//  
    }

    if(battery_data==1)
    {
  display.setCursor(0,25);    
  display.print("Ch= ");
  for(int i=3;i<=4;i++)            
  display.print(SM_ARU_1_BT[i]);// Battery   
  display.print("%");
    }

if(alarm_data==1)
{
  display.setCursor(50,25);    
  display.print(" Alarm= ");
  for(int i=3;i<=7;i++)            
  display.print(SM_ARU_5_AL[i]);// Alarm


  
  a=SM_ARU_5_AL[3]-0x30;
  b=SM_ARU_5_AL[4]-0x30;
  c=SM_ARU_5_AL[6]-0x30;
  d=SM_ARU_5_AL[7]-0x30;
  
  alarm_time=((a*1000)+(b*100)+(c*10)+d);
  
//  Serial.print("alarm_time");
//  Serial.println(alarm_time);
  
  if(current_time==alarm_time)
  {    
      Serial.println("Alarm Match");
      display.setCursor(0,15);  
      display.print("R= ");  
      for(int i=15;SM_ARU_5_AL[i]!='#';i++)   //alarm message         
      display.print(SM_ARU_5_AL[i]);
      
      digitalWrite(Buzzer, 0);  
  }
  else 
      digitalWrite(Buzzer, 1);
}
if(alarm_data==0) digitalWrite(Buzzer, 1);

  display.display();
 
    delay(1);     
}

void Display_1()// Date Time Battert Net Alarm
{
  display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0,2);
  for(int i=3;i<=21;i++)            
  display.print(SM_ARU_0_DA[i]);// Date & Time



  display.setCursor(0,12);
  
  Serial.println(SM_ARU_4_NE[3]);          
  if(SM_ARU_4_NE[3]=='C')display.print("Internet Connected");// Internet
  if(SM_ARU_4_NE[3]=='D')display.print("Internet Disconnected");// Internet
  


  display.setCursor(0,25);    
  display.print("Ch= ");
  for(int i=3;i<=4;i++)            
  display.print(SM_ARU_1_BT[i]);// Battery   
  display.print("%");
  
  
  display.setCursor(50,25);    
  display.print(" Alarm= ");
  for(int i=3;i<=7;i++)            
  display.print(SM_ARU_5_AL[i]);// Alarm
  
  display.display();
      
}

void Display_3() // Call
{

String SM_ARU_3_Call="";
    display.clearDisplay();
    
    display.setTextSize(1);             
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);            
    display.println("Call");
    
    for(int i=15;i<=24;i++)            
    SM_ARU_3_Call+=(SM_ARU_3_CA[i]);
                
    display.print(SM_ARU_3_Call);

    display.setCursor(0,20);              
    if(SM_ARU_3_Call=="9834049482")display.println("Zero@3327");
    
    display.display();
    delay(1000);
}

void Display_2()  //Message
{
  display.clearDisplay();
  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);            
  //display.println("Message ");
  for(int i=12;SM_ARU_2_ME[i]!='#';i++)            
  display.print(SM_ARU_2_ME[i]);
  display.display();
  delay(1000);

}
//////////////////////////////////////////////////////////////////////////////////////
void Display_Update_Time()
{
  display.setCursor(0,2);
  for(int i=3;i<=5;i++)            
  display.print(SM_ARU_0_DA[i]);// Date & Time

  display.print(" ");
  
  for(int i=16;i<=17;i++)            
  display.print(SM_ARU_0_DA[i]);//Day
  display.print('-');
  for(int i=13;i<=14;i++)            
  display.print(SM_ARU_0_DA[i]);//Month
  display.print('-'); 
  for(int i=10;i<=11;i++)            
  display.print(SM_ARU_0_DA[i]);//year
  
  display.print(" ");
  for(int i=19;i<=20;i++)            
  display.print(SM_ARU_0_DA[i]);//HH 
  display.print(':');  
  for(int i=22;i<=23;i++)            
  display.print(SM_ARU_0_DA[i]);//MM

  a=SM_ARU_0_DA[19]-0x30;
  b=SM_ARU_0_DA[20]-0x30;
  c=SM_ARU_0_DA[22]-0x30;
  d=SM_ARU_0_DA[23]-0x30;
  
  current_time=((a*1000)+(b*100)+(c*10)+d);
  Serial.print("current_time");
  Serial.println(current_time); 
}

void Display_Update_Internet()
{

  display.setCursor(115,2);
           
  if(SM_ARU_4_NE[3]=='C')display.print("E");// Internet
  if(SM_ARU_4_NE[3]=='D')display.print("-");// Internet
  
}

void Display_Update_Battery()
{
  display.setCursor(0,25);    
  display.print("Ch= ");
  for(int i=3;i<=4;i++)            
  display.print(SM_ARU_1_BT[i]);// Battery   
  display.print("%");
}

void Display_Update_Alarm()
{
  display.setCursor(50,25);    
  display.print(" Alarm= ");
  for(int i=3;i<=7;i++)            
  display.print(SM_ARU_5_AL[i]);// Alarm
  
  a=SM_ARU_5_AL[3]-0x30;
  b=SM_ARU_5_AL[4]-0x30;
  c=SM_ARU_5_AL[6]-0x30;
  d=SM_ARU_5_AL[7]-0x30;
  
  alarm_time=((a*1000)+(b*100)+(c*10)+d);
  
  Serial.print("alarm_time");
  Serial.println(alarm_time); 
  if(current_time==alarm_time)
  {    
      Serial.println("Alarm Match");
      display.setCursor(0,15);  
      display.print("R= ");  
      for(int i=15;SM_ARU_5_AL[i]!='#';i++)   //alarm message         
      display.print(SM_ARU_5_AL[i]);
      
      digitalWrite(Buzzer, 0);
      delay(100);
      digitalWrite(Buzzer, 1);
  }
  display.display();
}
