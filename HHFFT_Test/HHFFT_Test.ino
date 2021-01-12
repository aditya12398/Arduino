#include <arduinoFFT.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

double ox , oy ; //for graph
double x , y ; //variables for graph
boolean graphEnabled = true ; //flag to plot the graph

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define CHANNEL A15
#define VCC 52
#define GND 53
#define g 9.81
const uint16_t samples = 512; //This value MUST ALWAYS be a power of 2
double samplingFrequency = 500;

unsigned int delayTime = 0;
int a = 0;

double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define TS_MINX 118
#define TS_MINY 135
#define TS_MAXX 921
#define TS_MAXY 896

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8  // can be a digital pin
#define XP 9   // can be a digital pin

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_SPITFT tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 30);

boolean buttonEnabled = true;

void setup() {
  Serial.begin(115200);
  Serial.print("Starting...");

   pinMode(VCC , OUTPUT);
  pinMode(GND , OUTPUT);
  pinMode(CHANNEL , INPUT);
  digitalWrite(VCC , HIGH);
  digitalWrite(GND , LOW);


    if(samplingFrequency<=1000)
    delayTime = 1000/samplingFrequency;
  else
    delayTime = 1000000/samplingFrequency;

  tft.reset();
  uint16_t identifier = tft.readID();
  identifier=0x9325;
  tft.begin(identifier);
  tft.setRotation(1);
   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
    tft.fillScreen(BLACK);

  //Draw white frame
  tft.drawRect(0,0,319,240,WHITE);

  //Print "Hello" Text
  tft.setCursor(100,30);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Hello");

  //Create Button for FFT Graph
  tft.fillRect(60,120,200,40,RED);
  tft.drawRect(60,120,200,40,WHITE);
  tft.setCursor(80,130);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("PLOT FFT GRAPH");


  //Create Button for Frequency
  tft.fillRect(60,180, 200, 40, RED);
  tft.drawRect(60,180,200,40,WHITE);
  tft.setCursor(80,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("GET FREQUENCY");


}

void loop()
{

  TSPoint p = ts.getPoint();  //Get touch point

  if (p.z > ts.pressureThreshhold) {

   p.x = map(p.x, TS_MAXX, TS_MINX, 320, 0);
   p.y = map(p.y, TS_MAXY, TS_MINY, 240, 0);


//   Serial.print("X = "); Serial.print(p.x);
//   Serial.print("\tY = "); Serial.print(p.y);
//   Serial.print("\n");
//



   if(p.x>60 && p.x<260 && p.y>180 && p.y<220 && buttonEnabled)// The user has pressed inside the red rectangle
   {
    buttonEnabled = false; //Disable button

    //This is important, because the libraries are sharing pins
   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
    tft.fillScreen(BLACK);
    tft.drawRect(0,0,319,240,WHITE);
    tft.setCursor(50,50);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Processing...");
    // Clearing the arrays
    for(uint16_t i=0;i<samples;i++)
    {
      vReal[i] = 0;
      vImag[i] = 0;
    }

     for(uint16_t i =0;i<samples;i++)
  {
    vReal[i] = double(analogRead(CHANNEL));
    vReal[i] = map(vReal[i] , 331 , 504 ,-1*g,g);
    if(samplingFrequency<=1000)
      delay(delayTime);
    else
      delayMicroseconds(delayTime);
  }

    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples,FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    double pk[3] ;
    FFT.MajorPeak(vReal, samples, samplingFrequency, pk);


    //Erase the screen
    tft.fillScreen(BLACK);

    //Draw frame
    tft.drawRect(0,0,319,240,WHITE);

    tft.setCursor(50,50);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.println("Frequency : ");
    for(int j =0;j<1;j++){
      tft.print("          ");
      tft.print(pk[j]);
      tft.println("Hz ");
    }

    tft.fillRect(250,0,70,40,RED);
    tft.drawRect(250,0,70,40,WHITE);
    tft.setCursor(260,10);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("BACK");
   }
   else if(p.x>250 && p.x<320 && p.y>0 && p.y<40 && buttonEnabled == false)//Clicking back button
   {
     buttonEnabled = true ;
     pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
    tft.fillScreen(BLACK);

  //Draw white frame
  tft.drawRect(0,0,319,240,WHITE);

  //Print "Hello" Text
  tft.setCursor(100,30);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Hello");

  //Create Button for FFT Graph
  tft.fillRect(60,120,200,40,RED);
  tft.drawRect(60,120,200,40,WHITE);
  tft.setCursor(80,130);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("PLOT FFT GRAPH");


  //Create Button for Frequency
  tft.fillRect(60,180, 200, 40, RED);
  tft.drawRect(60,180,200,40,WHITE);
  tft.setCursor(80,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("GET FREQUENCY");

   }
else if(p.x>60 && p.x<260 && p.y>120 && p.y<160 && buttonEnabled == true)
{
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  graphEnabled = true ;
  buttonEnabled = false ;
  tft.fillScreen(BLACK);
  tft.drawRect(0,0,319,240,WHITE);
  tft.setCursor(50,50);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Processing...");


  // Clearing the arrays
    for(uint16_t i=0;i<samples;i++)
    {
      vReal[i] = 0;
      vImag[i] = 0;
    }

     for(uint16_t i =0;i<samples;i++)
  {
    vReal[i] = double(analogRead(CHANNEL));
    vReal[i] = map(vReal[i] , 331 , 504 ,-1*g,g);
    if(samplingFrequency<=1000)
      delay(delayTime);
    else
      delayMicroseconds(delayTime);
  }

    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples,FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

    double pk[3] ;
    FFT.MajorPeak(vReal, samples, samplingFrequency, pk);
    int indexMax = pk[0]*samples/samplingFrequency ;


    tft.fillScreen(BLACK);

    for(uint16_t i=5;i<samples/2;i++)
    {
      x = i*samplingFrequency/samples ;
      y = vReal[i];
      Graph(tft, x, y, 50, 200, 250, 150, 0, samplingFrequency/2, 25, 0, vReal[indexMax], vReal[indexMax]/10, "FFT", "Frequency", "Amplitude", BLUE, RED, WHITE, WHITE, BLACK, graphEnabled);

    }
    tft.fillRect(250,0,70,40,RED);
    tft.drawRect(250,0,70,40,WHITE);
    tft.setCursor(260,10);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("BACK");




}

   delay(10);
  }
}


void Graph(Adafruit_SPITFT &d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {

  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;

  if (redraw == true) {

    redraw = false;
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else {
        d.drawLine(gx, temp, gx + w, temp, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(gx - 40, temp);
      // precision is default Arduino--this could really use some format control
      d.println(int(i));
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform

      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, gy + 10);
      // precision is default Arduino--this could really use some format control
      d.println(int(i));
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(gx , gy - h - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 30, gy - h - 10);
    d.println(ylabel);


  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, pcolor);
  d.drawLine(ox, oy + 1, x, y + 1, pcolor);
  d.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}
