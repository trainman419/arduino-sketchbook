#include <Bounce2.h>
#include <Wire.h>
#include <MAX17043.h>

const int left[] = {6, 3, 9, 5}; // 6, 3, 9, 5
const int right[] = {4, 7, 2, 8};
const int yellow_l = 10;
const int yellow_r = 11;

const int up_pin = A0;
const int down_pin = 12;

byte i=0;
byte dir=0;

// modes
#define MIN_MODE 0
#define COUNT 0
#define GAUGE 1
#define TWINKLE 2
#define MAX_MODE 2
int mode = COUNT;

MAX17043 fuelGauge;

Bounce up_btn;
Bounce down_btn;

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
  
  pinMode(up_pin, INPUT);
  digitalWrite(up_pin, HIGH); // enable pull-up
  up_btn.attach(up_pin);
  up_btn.interval(20);
  
  pinMode(down_pin, INPUT);
  digitalWrite(down_pin, HIGH); // enable pull-up
  down_btn.attach(down_pin);
  down_btn.interval(20);
  
  fuelGauge.begin();

  Serial.begin(9600);
  
  // startup sequence - like a light blinking on
  // ..*. .... .*.. ...* ..*. .... **.* ....
  uint32_t states = 0b00001011000001001000001000000100;
  for(int i=0; i<32; i++ ) {
    if( states & 0b1 ) {
      left_bar(1024);
      right_bar(4);
      analogWrite(yellow_l, 255);
      analogWrite(yellow_r, 255);
    } else {
      left_bar(0);
      right_bar(0);
      analogWrite(yellow_l, 0);
      analogWrite(yellow_r, 0);
    }
    delay(30);
    states >>= 1;
  }
}

int left_rand = 0;
int right_rand = 0;
long rand_millis = 0;

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
      if( rand_millis < millis() ) {
        int r_range = random(10) > 7 ? 5 : 3;
        left_rand = constrain(left_rand + random(512) - 256, 0, 1024);
        right_rand = constrain(right_rand + random(r_range) - 1, 0, 4);
        left_bar(left_rand);
        right_bar(right_rand);
        rand_millis += 250;
      }
      break;
  }
  
  if( up_btn.update() && !up_btn.read() ) {
    mode = min(mode+1, MAX_MODE);
  }

  if( down_btn.update() && !down_btn.read() ) {
    mode = max(mode-1, MIN_MODE);
  }
  
  i += dir;
  if( i == 255 ) dir = -1;
  if( i == 0 ) dir = 1;
}
