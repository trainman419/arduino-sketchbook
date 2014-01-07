/*
  Spacebrew Range

 Demonstrates how to create a sketch that sends and receives analog
 range value to and from Spacebrew. Every time the state of the
 potentiometer (or other analog input component) change a spacebrew
 message is sent. The sketch also accepts analog range messages from
 other Spacebrew apps.

 Make sure that your Yun is connected to the internet for this example
 to function properly.

 The circuit:
 - Potentiometer connected to Yun. Middle pin connected to analog pin A0,
   other pins connected to 5v and GND pins.

 created 2013
 by Julio Terra

 This example code is in the public domain.

 More information about Spacebrew is available at:
 http://spacebrew.cc/

 */
 
 /*
          Rotary_Encoder_LED_Ring_Example
          www.mayhewlabs.com/products/rotary-encoder-led-ring 
          Copyright (c) 2011 Mayhew Labs.
          Written by Mark Mayhew
          
This example shows 3 sequences that are possible on the ring of LEDs around the encoder based on rotation of an encoder.  The 3 sequences are 
selected by entering 1, 2, or 3 in the serial command prompt.  The serial window shows the current rotary encoder count.  As the encoder is turned, 
the serial display shows a raw count of the rotary encoder's value and the LEDs show a scaled version of the value.  If the button on the rotary 
encoder is pushed, the bottom LED will come on.  Each section of code below discusses the process required to do this. 

A note on setting the output sequence:
Think of each LED as a single bit in a 16-bit long binary string.  If a bit is 1, the LED is on, if a bit is 0, the LED is off.  
By making a string of ones and zeros, we choose which LEDs to have on and off, and then send this string to the shift register to display it.
For example 1000000000000001 binary (0x8001 in hex) will have the fist and last LEDs on, the rest off.  

CREDIT:
Reading the rotary encoder is performed with Oleg's example code:a
http://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino
*/


#include <Bridge.h>
#include <SpacebrewYun.h>

// create a variable of type SpacebrewYun and initialize it with the constructor
SpacebrewYun sb = SpacebrewYun("Spacebrew Knob", "Spacebrew-attached encoder knob");

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

/*This is a 2 dimensional array with 3 LED sequences.  The outer array is the sequence; 
  the inner arrays are the values to output at each step of each sequence.  The output values
  are 16 bit hex values (hex math is actually easier here!).  An LED will be on if its 
  corresponding binary bit is a one, for example: 0x7 = 0000000000000111 and the first 3 LEDs 
  will be on.
  
  The data type must be 'unsigned int' if the sequence uses the bottom LED since it's value is 0x8000 (out of range for signed int).
*/
const unsigned int sequence[16] = {0x0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff};

long reset_start = 0;

void setup()
{
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
    
  // start the serial port
  Serial.begin(57600);

  // start-up the bridge
  Bridge.begin();

  // configure the spacebrew object to print status messages to serial
  sb.verbose(true);

  // configure the spacebrew publisher and subscriber
  sb.addPublish("knob", "range");
  sb.addPublish("button", "boolean");
  
  // subscribe to input
  sb.addSubscribe("in", "range");
  
  sb.onRangeMessage(handleRange);

  // connect to cloud spacebrew server at "sandbox.spacebrew.cc"
  sb.connect("sandbox.spacebrew.cc");
  
  update_disp(0xFFFF); // initialize display to 0
  
  delay(1000);
  
  update_disp(0);

  //Set serial rate, prompt for desired sequence
  Serial.println("Lightswitch Ready!");
  reset_start = millis();
}

// Global variables
int scaledCounter = 0;  //The LED output is based on a scaled veryson of the rotary encoder counter
int incomingByte = 0;   //Serial input to select LED output sequence

volatile int8_t knob_counter = 0;
int8_t disp_counter = 0;

#define LOOP_RATE 25
#define SLEEP_TIME (1000 / LOOP_RATE)

void loop()
{ 
  //Local Variables
  static int8_t last_counter = 0;
  static uint16_t disp = 0;
  static uint16_t disp_old = 0;
  int8_t counter = knob_counter;
  long start = millis();
  
  if( counter != last_counter ) {
    //Print out the counter value
    Serial.print("Counter value: ");
    Serial.println(counter, DEC);
    // connected to spacebrew then send a new value whenever the pot value changes
    if ( sb.connected() ) {
      sb.send("knob", counter * 17);
    }
    last_counter = counter;
  }
    
  // monitor spacebrew connection for new data
  sb.monitor();
  
  disp_counter = counter;
  
  //Send the LED output to the shift register
  disp = sequence[disp_counter / 4];
  
  long now = millis();
  
  //If the encoder switch is pushed, this will turn on the bottom LED.  The bottom LED is turned
  //on by 'OR-ing' the current display with 0x8000 (1000000000000000 in binary)
  static byte sw_old = false;
  byte sw = !digitalRead(ENC_SW);
  if (sw)
  {
    disp |= 0x8000;
  } else {
    disp &= ~0x8000;
    reset_start = now;
  }
  
  if( sw != sw_old ) {
    sb.send("button", sw);
    sw_old = sw;
  }
  
  if( now - reset_start > 3000 ) {
    // Turn the relay off and back on to reset the lights
    digitalWrite(RELAY, HIGH);
    delay(1000);
    digitalWrite(RELAY, LOW);
    reset_start = millis();
  }
  
  // update the display if needed
  if( disp != disp_old ) {
    disp_old = disp;
    update_disp(disp);
  }

  now = millis();
  // now - start seems to range between 4 and 20
  // values in the 100-150 range when sending updates
  if( (now - start) < SLEEP_TIME ) {
    delay(SLEEP_TIME - (now - start));
  }
}

void update_disp(uint16_t disp) {
  digitalWrite(LE,LOW);  
  shiftOut(SDI,CLK,MSBFIRST,disp >> 8);
  shiftOut(SDI,CLK,MSBFIRST,disp);              
  digitalWrite(LE,HIGH);   
}


// TODO: handle encoders in interrupts

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
  knob_counter += change;
  // enforce limits on counter
  if( knob_counter < 0  ) knob_counter = 0;
  if( knob_counter > 60 ) knob_counter = 60;
}

void handleRange(String route, int value) {
  disp_counter = value / 17;
}
