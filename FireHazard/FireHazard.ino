#include <Servo.h>

void LeftDir(bool fwd) {
  digitalWrite(4, fwd);
  digitalWrite(5, !fwd);
}

void RightDir(bool fwd) {
  digitalWrite(7, fwd);
  digitalWrite(8, !fwd);
}

void LeftOut(int8_t pwr) {
  LeftDir(pwr > 0);
  analogWrite(6, abs(pwr)*2);
}

void RightOut(int8_t pwr) {
  RightDir(pwr > 0);
  analogWrite(11, abs(pwr)*2);
}

int vel = 0;  // (-50 bwd, +50 fwd)
int turn = 0; // (-50 left, +50 right)

void Mix() {
  int l = 2*vel + turn;
  int r = 2*vel - turn;
  l = min(max(l, -120), 120);
  r = min(max(r, -120), 120);
  LeftOut(l);
  RightOut(r);
}

Servo weapon;

void Weapon(int pwr) {
  weapon.write(pwr);
  Serial.println(pwr);
}

long last_command = 0;

void setup() {
  // put your setup code here, to run once:
  
  // motor setup
  digitalWrite(6, LOW);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  
  digitalWrite(11, LOW);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  Serial.begin(57600);
  
  // weapon setup
  weapon.attach(3);
  //Weapon(0);
  Weapon(0);
  
  delay(6000);
  
  Weapon(30);
  
  delay(2000);
  Weapon(0);
  
  delay(1000);
  
  Weapon(65);
    
  RightOut(20);
  LeftOut(20);
  delay(250);
  RightOut(0);
  LeftOut(0);
  
  //Weapon(0);
}

int i = 60;

char mode = 0;
int value = 0;

void loop() {
  // put your main code here, to run repeatedly:

  // read joystick data
  // E: (99-0) (weapon speed)
  // B: (150-250) (left to right)
  // A: (150-250) (bwd to fwd)
  while(Serial.available()) {
    int b = Serial.read();
    if( 'A' <= b && 'X' >= b ) {
      switch(mode) {
        case 'A':
          vel = (value - 205);
          break;
        case 'B':
          turn = (value - 200);
          Mix();
          break;
        case 'E':
          Weapon(100 - value);
          break;
      }
          
      // mode character
      mode = b;
      value = 0;
    } else if('0' <= b && '9' >= b) {
      value *= 10;
      value += (b - '0');
    }
    last_command = millis();
  }
  
  // timeout
  if((millis() - last_command) > 500) {
    LeftOut(0);
    RightOut(0);
    Weapon(0);
  }
}
