/*
  SD Slots:
    MISO 50
    MOSI 51
    SCK 52
    CS 53
  Calliration Constants:
    1 ton = -12.88623
    4 ton = -51.4
    10 ton = -128.8623
*/

#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "LiquidCrystal.h"
#include <Wire.h>

RTC_DS1307 rtc;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

int i, sign, clockpin1 = 17, clockpin2 = 4, datapin1 = 16, datapin2 = 3;
long value;
float result1, result2, c10=-128.8623, c4=-51.4, c1=-12.88623;

unsigned long tempmicros;
String timestamp, title, subtitle;
File myFile;

void setup() {
  lcd.setCursor(0,0);
  Serial.begin(57600);
  if (! rtc.begin()) {
    lcd.print("Couldn't find RTC");
    while (1);
  }
  Serial.begin(9600);
  pinMode(clockpin1, INPUT);
  pinMode(datapin1, INPUT);
  pinMode(clockpin2, INPUT);
  pinMode(datapin2, INPUT);
  lcd.begin(20, 4);  // initializes the 20x4 LCD
  lcd.setCursor(0,0);
  if (! rtc.isrunning()) {
    lcd.print("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2018, 8, 18, 16, 12, 0));
  }

  DateTime now = rtc.now();
  subtitle =String(now.day(), DEC)+ String(now.month(), DEC)+ String(now.hour(), DEC) + String(now.minute(), DEC);
  title =String(subtitle) + String(".csv");
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/
  lcd.setCursor(0,1);
  if (!SD.begin(53)) {
    lcd.print("SD init. failed!");
    return;
  }
  else{
    timestamp = String(now.day(), DEC)+ String('/')+ String(now.month(), DEC) + String('/') + String(now.year(), DEC)+
    String("|")+
    String(now.hour(), DEC) + String(':')+ String(now.minute(), DEC) + String(':') + String(now.second(), DEC);
    myFile = SD.open(title, FILE_WRITE);
    lcd.print("SD initialized");
    myFile.print("File set up, Started at:, ");
    myFile.println(timestamp);
    myFile.println("Timestamp, 1 Ton Scale, 4 Ton Scale, 10 Ton Scale, Displacement");
  }
  lcd.setCursor(0,2);
  lcd.print("Filename:");
  lcd.print(String(title));
  myFile.close();
  //lcd.setCursor(19,3);
  delay(8000);
  lcd.clear();
}

void loop() {
  myFile = SD.open(title, FILE_WRITE);
  DateTime now = rtc.now();

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

  timestamp = String(now.day(), DEC)+ String('/')+ String(now.month(), DEC) + String('/') + String(now.year(), DEC)+
  String("|")+
  String(now.hour(), DEC) + String(':')+ String(now.minute(), DEC) + String(':') + String(now.second(), DEC);

  myFile.print(timestamp);
  myFile.print(", ");
  myFile.print((result1*c1), 2);
  myFile.print(", ");
  myFile.print((result1*c4), 2);
  myFile.print(", ");
  myFile.print((result1*c10), 2);
  myFile.print(", ");
  myFile.println(result2, 2);
  lcd.setCursor(0,0);
  lcd.print(timestamp);
  lcd.setCursor(0,1);
  lcd.print("1T:");
  lcd.print((result1*c1), 2);
  lcd.print(" 4T:");
  lcd.print((result1*c4), 2);
  lcd.setCursor(0,2);
  lcd.print("10T:");
  lcd.print((result1*c10), 2);
  lcd.print(" D:");
  lcd.print((result2), 2);
  lcd.setCursor (0,3);
  lcd.print("File: ");
  lcd.print(String(title));
  myFile.close();
  //delay(20);
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
