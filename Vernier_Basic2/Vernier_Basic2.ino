int i;
int sign;
long value;
float result1, result2;
int clockpin1 = 12;
int clockpin2 = 10;
int datapin1 = 13;
int datapin2 = 11;
unsigned long tempmicros;

void setup() {
  Serial.begin(9600);
  pinMode(clockpin1, INPUT);
  pinMode(datapin1, INPUT);
}
void loop ()
{
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
 delay(1000);
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
  Serial.println(result1,2); //print result1 with 2 decimals
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
  Serial.println(result2,2); //print result2 with 2 decimals
}
