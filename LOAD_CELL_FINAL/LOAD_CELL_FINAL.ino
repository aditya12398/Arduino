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

#include "HX711.h"  //You must have this library in your arduino library folder
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
//#include "LiquidCrystal.h"

#define DOUT  3
#define CLK  2
#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;
//LiquidCrystal lcd(11, 10, 5, 4, 3, 2);  // sets the interfacing pins

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
HX711 scale(DOUT, CLK);

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = -106600; //-106600 worked for my 40Kg max scale setup
int i;
int sign;
long value;
float result;
int clockpin = 12;
int datapin = 13;
unsigned long tempmicros;

void setup() {
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.begin(9600);
  pinMode(clockpin, INPUT);
  pinMode(datapin, INPUT);

  //lcd.begin(16, 2);  // initializes the 16x2 LCD
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
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
    Serial.println("initialization failed!");
    return;
  }
  else
  Serial.println("initialization done.");

  Serial.println("Press T to tare");
  scale.set_scale(-106600);  //Calibration Factor obtained from first sketch
  scale.tare();             //Reset the scale to 0
}

void loop() {
  while (digitalRead(clockpin)==HIGH) {} //if clock is LOW wait until it turns to HIGH
  tempmicros=micros();
  while (digitalRead(clockpin)==LOW) {} //wait for the end of the HIGH pulse
  if ((micros()-tempmicros)>500) { //if the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    decode(); //decode the bit sequence
  }
  File myFile = SD.open("LoadCell.txt", FILE_WRITE);
  DateTime now = rtc.now();
/*
  lcd.setCursor(0,0);           //sets the cursor at row 0 column 0
  lcd.print("16x2 LCD MODULE"); // prints 16x2 LCD MODULE
  lcd.setCursor(2,1);           //sets the cursor at row 1 column 2
  lcd.print("HELLO WORLD");     // prints HELLO WORLD
  */
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" ");
  Serial.print("Weight: ");
  Serial.print(scale.get_units(), 3);  //Up to 3 decimal points
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow lbs
  Serial.print(", Displacement: ");
  Serial.print(result, 2);
  Serial.println(" mm");

  myFile.print(now.year(), DEC);
  myFile.print('/');
  myFile.print(now.month(), DEC);
  myFile.print('/');
  myFile.print(now.day(), DEC);
  myFile.print(" | ");
  myFile.print(now.hour(), DEC);
  myFile.print(':');
  myFile.print(now.minute(), DEC);
  myFile.print(':');
  myFile.print(now.second(), DEC);
  myFile.print(", ");
  myFile.print(scale.get_units(), 3);  //Up to 3 decimal points
  myFile.print(", ");
  myFile.println(result, 2);

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == 't' || temp == 'T')
      scale.tare();  //Reset the scale to zero
  }
  delay(1000);
  myFile.close();
}
void decode() {
  sign=1;
  value=0;
  for (i=0;i<23;i++) {
    while (digitalRead(clockpin)==HIGH) { } //wait until clock returns to HIGH- the first bit is not needed
    while (digitalRead(clockpin)==LOW) {} //wait until clock returns to LOW
    if (digitalRead(datapin)==LOW) {
      if (i<20) {
        value|= 1<<i;
      }
      if (i==20) {
        sign=-1;
      }
    }
  }
  result=(value*sign)/100.00;
}
