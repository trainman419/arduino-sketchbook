/*
  Running shell commands using Process class.

 This sketch demonstrate how to run linux shell commands
 using an Arduino Yún. It runs the wifiCheck script on the linino side
 of the Yun, then uses grep to get just the signal strength line.
 Then it uses parseInt() to read the wifi signal strength as an integer,
 and finally uses that number to fade an LED using analogWrite().

 The circuit:
 * Arduino Yun with LED connected to pin 9

 created 12 Jun 2013
 by Cristian Maglie
 modified 25 June 2013
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/ShellCommands

 */

#include <Process.h>

//These are the pins that will talk to the shift register through SPI
#define SDI    5    //Data
#define CLK    6    //Clock
#define LE     7    //Latch

//These are the rotary encoder pins A, B, and switch
#define ENC_A    2
#define ENC_A_INT 1
#define ENC_B    3
#define ENC_B_INT 0
#define ENC_SW   4

// This is the pin that the relay contol circuit is hooked up to
#define RELAY 8

// LED sequence
const unsigned int sequence[16] = {0x0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff};

long reset_start = 0;

Process p;

void setup() {
  //Set SPI pins to output
  pinMode(SDI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LE,OUTPUT);
  //Set encoder pins to input, turn internal pull-ups on
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  attachInterrupt(ENC_A_INT, update_counter, CHANGE);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);
  attachInterrupt(ENC_B_INT, update_counter, CHANGE);
  
  pinMode(ENC_SW, INPUT);
  digitalWrite(ENC_SW, HIGH);
  pinMode(RELAY, OUTPUT);
  
  digitalWrite(RELAY, LOW);
  
  Bridge.begin();	// Initialize the Bridge
  
  update_disp(0xFFFF); // flash lights on and off
  
  delay(500);
  
  update_disp(0);

  //Set serial rate, prompt for desired sequence
  reset_start = millis();
}

volatile int16_t counter = 0;

#define LOOP_RATE 25
#define SLEEP_TIME (1000 / LOOP_RATE)

void loop() {
  static int16_t last_counter = 0;
  static uint16_t disp_old = 0;
  static char out[5];
  static long last_input = 0;
  static int16_t last_output = 0;
  
  if(!p.running()) {
    // if our process has died, restart it
    p.begin("/root/hueyun.py");
    p.addParameter("Left of couch 1");
    p.addParameter("Right of couch 1");
    p.runAsynchronously();
    p.setTimeout(50);
  }
  
  // Read command output. runShellCommand() should have passed "Signal: xx&":
  long now = millis();
  int latest = counter;
  if( latest != last_counter ) {
    last_counter = latest;
    last_input = now;
  }
  
  // coalesce output
  if( last_output != latest ) {
    if( now - last_input > 250 ) {
      // this updates the lights
      p.print(latest * 4);
      p.print("\r\n");
      last_output = latest;    
    }
  }

  //If the encoder switch is pushed, this will turn on the bottom LED.  The bottom LED is turned
  //on by 'OR-ing' the current display with 0x8000 (1000000000000000 in binary)
  static byte sw_old = false;
  byte sw = !digitalRead(ENC_SW);

  if(!sw)
    reset_start = now;
  
  if( sw != sw_old ) {
    // on button release, send "T" to toggle lights
    if( sw_old )
      // try to send enough data to fill up the input buffer
      p.print("T\r\n");
    sw_old = sw;
  }
  
  if( now - reset_start > 3000 ) {
    // Turn the relay off and back on to reset the lights
    digitalWrite(RELAY, HIGH);
    delay(1000);
    digitalWrite(RELAY, LOW);
    reset_start = millis();
  }

  static int16_t input = 0;
  static int8_t input_count = 0;
  while (p.available()) {
    char b = p.read();
    if(b == '\n' || b == '\r') {
      if( input_count > 0 ) {
        latest = input / 4;
        if(latest > 60) latest = 60;
        if(latest < 0) latest = 0;      
        counter = latest;
        input = 0;
        input_count = 0;
      }
    } else if(isdigit(b)) {
      input = input * 10;
      input += (b - '0');
      input_count++;
    }
  }
  
  
  uint16_t disp = 0;
  // scale to 0-15
  disp = sequence[latest / 4];

  if (sw)
    disp |= 0x8000;
  else
    disp &= ~0x8000;
    
  if( disp != disp_old ) {
    update_disp(disp);
    disp_old = disp;
  }
}


void update_disp(uint16_t disp) {
  digitalWrite(LE,LOW);  
  shiftOut(SDI,CLK,MSBFIRST,disp >> 8);
  shiftOut(SDI,CLK,MSBFIRST,disp);              
  digitalWrite(LE,HIGH);   
}

byte raw_enc() {
  byte bit0 = digitalRead(ENC_A);
  byte bit1 = digitalRead(ENC_B);
  return bit0 | (bit1 << 1);
}
/*************************************************************************
*    read_encoder() function as provided by Oleg:                        *
*    http://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino *
*                                                                        *
*    Returns change in encoder state (-1,0,1)                            *
************************************************************************ */
int8_t read_encoder()
{
  int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  old_AB |= raw_enc();  //add current state
  return ( enc_states[( old_AB & 0x0f )]);
}

void update_counter() {
  int8_t change = read_encoder();
  counter += change;
  // enforce limits on counter
  if( counter < 0  ) counter = 0;
  if( counter > 60 ) counter = 60;
}
