/*
 * https://circuits4you.com
 * 2016 November 25
 * Load Cell HX711 Module Interface with Arduino to measure weight in Kgs
 Arduino
 pin
 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/

#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "LiquidCrystal.h"
#include <Wire.h>

RTC_DS1307 rtc;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);  // sets the interfacing pins

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
int i;
int sign;
long value;
float result1, result2, c=-128.8623; //;1 ton = -12.88623;10 ton = -128.8623 4 ton = -51.4
int clockpin1 = 17;
int clockpin2 = 4;
int datapin1 = 16;
int datapin2 = 3;
unsigned long tempmicros;
String timestamp;

void setup() {
  Serial.begin(57600);
  if (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.begin(9600);
  pinMode(clockpin1, INPUT);
  pinMode(datapin1, INPUT);
  pinMode(clockpin2, INPUT);
  pinMode(datapin2, INPUT);

  lcd.begin(20, 4);  // initializes the 20x4 LCD

  if (! rtc.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2018, 8, 18, 16, 12, 0));
  }

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    //Serial.println("initialization failed!");
    return;
  }
  //else
  //Serial.println("initialization done.");

  //Serial.println("Press T to tare");
}

void loop() {

 while (digitalRead(clockpin1)==HIGH) {} //if clock is LOW wait until it turns to HIGH
 tempmicros=micros();
 while (digitalRead(clockpin1)==LOW) {} //wait for the end of the HIGH pulse
 if ((micros()-tempmicros)>500){ //if the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
   decode1(); //decode1 the bit sequence
  }

 while (digitalRead(clockpin2)==HIGH) {} //if clock is LOW wait until it turns to HIGH
 tempmicros=micros();
 while (digitalRead(clockpin2)==LOW) {} //wait for the end of the HIGH pulse
 if ((micros()-tempmicros)>500){ //if the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
   decode2(); //decode2 the bit sequence
  }

  File myFile = SD.open("Vernier.csv", FILE_WRITE);
  DateTime now = rtc.now();

  timestamp = String(now.day(), DEC)+ String('/')+ String(now.month(), DEC) + String('/') + String(now.year(), DEC)+
  String("|")+
  String(now.hour(), DEC) + String(':')+ String(now.minute(), DEC) + String(':') + String(now.second(), DEC);

  /*Serial.print("Time: " + timestamp);
  Serial.print("\tDisplacement 1: ");
  Serial.print(result1, 2);
  Serial.print("\tDisplacement 2: ");
  Serial.println(result2,2); //print result2 with 2 decimals*/
  myFile.print(timestamp);
  myFile.print(", ");
  myFile.print((result1*c), 2);
  myFile.print(", ");
  myFile.println(result2, 2);
  lcd.setCursor(0,0);           //sets the cursor at row 0 column 0
  lcd.print(timestamp); // prints 16x2 LCD MODULE
  lcd.setCursor(0,1);           //sets the cursor at row 1 column 2
  lcd.print("Load: ");
  lcd.print((result1*c), 2);
  lcd.setCursor(0,2);
  lcd.print("Displacement: ");
  lcd.println((result2), 2);
  lcd.setCursor (19,3);
  myFile.close();
  delay(20);
}
void decode1() {
  sign=1;
  value=0;
  for (i=0;i<23;i++) {
    while (digitalRead(clockpin1)==HIGH) { } //wait until clock returns to HIGH- the first bit is not needed
    while (digitalRead(clockpin1)==LOW) {} //wait until clock returns to LOW
    if (digitalRead(datapin1)==LOW) {
      if (i<20) {
        value|= 1<<i;
      }
      if (i==20) {
        sign=-1;
      }
    }
  }
  result1=(value*sign)/100.00;
  //Serial.println(result1,2); //print result1 with 2 decimals
}

void decode2() {
  sign=1;
  value=0;
  for (i=0;i<23;i++) {
    while (digitalRead(clockpin2)==HIGH) { } //wait until clock returns to HIGH- the first bit is not needed
    while (digitalRead(clockpin2)==LOW) {} //wait until clock returns to LOW
    if (digitalRead(datapin2)==LOW) {
      if (i<20) {
        value|= 1<<i;
      }
      if (i==20) {
        sign=-1;
      }
    }
  }
  result2=(value*sign)/100.00;
}
