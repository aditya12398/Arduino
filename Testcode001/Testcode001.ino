#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];
// Initialise the display
UTFT myGLCD(ILI9486,38,39,40,41);

void setup() {
  randomSeed(analogRead(0));
  //Serial.begin(9600);
// Setup the LCD
  myGLCD.InitLCD(1);
  myGLCD.setFont(SmallFont);
  }

void loop() {
  myGLCD.clrScr();
  int xsz, ysz;
  xsz = myGLCD.getDisplayXSize();
  ysz = myGLCD.getDisplayYSize();
  
  /*Serial.print("X size is: ");
  Serial.println(String(xsz));
  Serial.print("Y size is: ");
  Serial.println(ysz);*/

  myGLCD.print("X size is: " , CENTER, 1);
  myGLCD.print(String(xsz), CENTER, 14);
  myGLCD.print("Y size is: ", CENTER, 27);
  myGLCD.print(String(ysz), CENTER, 40);
  myGLCD.print("Shilpa Kitni Sundar hai *_*", CENTER, 53);
  for (int i = 1; i < 100; i++)
  {
    myGLCD.drawPixel (i, 320 - 2 * i);
    myGLCD.drawPixel (480 - i, 320 - 2 * i);
  }
  //delay(20000);
}
