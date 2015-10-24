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
  
  Serial.begin(9600);
  
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

void loop() {
  // put your main code here, to run repeatedly:

  // TODO: read joystick data
  // timeout
  /*
  if((millis() - last_command) > 500) {
    LeftOut(0);
    RightOut(0);
    Weapon(0);
  }
  */
  //Weapon(i);
  i += 5;
  if(i > 80)  i = 0;
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
}
