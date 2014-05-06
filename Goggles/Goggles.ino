#include <Wire.h>
#include <MAX17043.h>

const int left[] = {6, 3, 9, 5}; // 6, 3, 9, 5
const int right[] = {4, 7, 2, 8};
const int yellow_l = 10;
const int yellow_r = 11;

const int up_btn = A0;
const int down_btn = 12;

byte i=0;
byte dir=0;

// modes
#define COUNT 0
#define GAUGE 1
#define TWINKLE 2
int mode = COUNT;

MAX17043 fuelGauge;

void left_bar(uint16_t value) {
  int j=0;
  for( j=0; j < value/256 && j < 4; j++ ) 
    digitalWrite(left[j], HIGH);
  if( j < 4 )
    analogWrite(left[j], value%256);
  for( j++ ; j<4; j++ )
    digitalWrite(left[j], LOW);
}

void right_bar(uint8_t value) {
  int j=0;
  Serial.print(value);
  for( j=0; j < value && j < 4; j++ )
    digitalWrite(right[j], HIGH);
  Serial.print(" "); Serial.println(j);
  for( ; j<4; j++ )
    digitalWrite(right[j], LOW);
}

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<4; i++) {
    pinMode(left[i], OUTPUT);
    pinMode(right[i], OUTPUT);
  }
  pinMode(yellow_l, OUTPUT);
  
  pinMode(yellow_r, OUTPUT);
  
  pinMode(up_btn, INPUT);
  digitalWrite(up_btn, HIGH); // enable pull-up
  
  pinMode(down_btn, INPUT);
  digitalWrite(down_btn, HIGH); // enable pull-up
  
  fuelGauge.begin();

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  float pct = fuelGauge.getBatteryPercentage() / 100.0;
  
  // left and right yellow are mirrors of each other
  analogWrite(yellow_l, i);
  analogWrite(yellow_r, 255-i);
  
  switch(mode) {
    default:
    case COUNT:
      left_bar(i*4);
      right_bar((i+54)/64);
      break;
    case GAUGE:
      left_bar(pct * 1023);
      right_bar(i/63);
      break;
    case TWINKLE:
      left_bar(random(1024));
      right_bar(random(5));
      delay(240);
      break;
  }
  
  int up = !digitalRead(up_btn);
  int down = !digitalRead(down_btn);
  
  i += dir;
  if( i == 255 ) dir = -1;
  if( i == 0 ) dir = 1;
}
