#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#define USE_ARDUINO_INTERRUPTS true   
#include <PulseSensorPlayground.h>   
#define DEBUG true
#include "Timer.h"  
Timer t;
PulseSensorPlayground pulseSensor;
SoftwareSerial SIM900(7, 8);   // rx, tx      
#define kpa2atm 0.00986923267
//LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
TinyGPS gps;
int val;
int val1;
int tempPin = A1;// temperature Sensor Pin
int pressurePin = A2;//pressure sensor pin
int pressure= 0;
int HBCount = 0;
int HBCheck = 0;
int TimeinSec = 0;


const int PulseWire = 0;       // PulseSensor WIRE connected to ANALOG PIN 0
const int LED13 = 13;          //  LED to PIN 13.
int Threshold = 550;  

void setup() {

  Serial.begin(9600);
  SIM900.begin(9600);  
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       
  pulseSensor.setThreshold(Threshold); 

  if (pulseSensor.begin()) {
   

  }
}
  


void loop(){
  int a=0;
  

  val = analogRead(tempPin);
  float mv = (val/1024.0)*5000;
  float temp = mv/10;

  val1 = analogRead(pressurePin);
  int pkPa = (val1/(1023+0.095))/0.009;
  int pm = (kpa2atm*pkPa*760);


int myBPM = pulseSensor.getBeatsPerMinute();  // returns BPM as an "int".
                                               
if (pulseSensor.sawStartOfBeat()) {            
 SIM900.println("Device Active!");
 if(myBPM>0)
    a=1;
 SIM900.print("BPM: ");                       
 SIM900.println(myBPM);                        // Print the value  of myBPM. 
}
  delay(20);      
 
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;


    Serial.print(myBPM);
    Serial.print("?");
    Serial.print(pm);
    Serial.print("?");
    Serial.print(temp);
    delay(300);


  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();

      if (gps.encode(c)) 
        newData = true;  
    }
  }
  if (newData && a==1)   
  {
    if(myBPM<60 || myBPM>110 || pm<80 || temp<36 ||temp>37.2 || pm>120){
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);   
    SIM900.print("AT+CMGF=1\r"); //command to send sms on gsm module getting auto executed
    delay(200);


    SIM900.println("AT + CMGS = \"+91 02133 219300\"");// hospitals sms hotline mobile number with country code
    delay(100);
    SIM900.print("-------------------SMS Alert--------------------");
    SIM900.print(" Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    SIM900.print("  Longitude = ");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    SIM900.print("\n   HB per Min  : ");
    SIM900.print(myBPM);
    SIM900.print("\n   Blood Pressure : ");
    SIM900.print(pm);
    SIM900.print("\n   Body temperature : ");
    SIM900.print(temp);
    delay(200);
    SIM900.println((char)26); //end of sms
    delay(200);
    SIM900.println();
    }

  }

    
          
}
