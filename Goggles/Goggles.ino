const int left[] = {6, 3, 9, 5}; // 6, 3, 9, 5
const int right[] = {4, 7, 2, 8};
const int yellow_l = 10;
const int yellow_r = 11;

byte i=0;
byte dir=1;

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<4; i++) {
    pinMode(left[i], OUTPUT);
    pinMode(right[i], OUTPUT);
  }
  pinMode(yellow_l, OUTPUT);
  
  pinMode(yellow_r, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  
  // left and right yellow are mirrors of each other
  analogWrite(yellow_l, i);
  analogWrite(yellow_r, 255-i);
  
  // right; digital only
  int j=0;
  for( j=0; j < i/64; j++ ) {
    digitalWrite(right[j], HIGH);
    digitalWrite(left[j], HIGH);
  }
  digitalWrite(right[j], HIGH);
  analogWrite(left[j], (i%64)*4);
  for( j++ ; j<4; j++ ) {
    digitalWrite(right[j], LOW);
    digitalWrite(left[j], LOW);
  }
  
  i += dir;
  if( i == 255 ) dir = -1;
  if( i == 0 ) dir = 1;
}
