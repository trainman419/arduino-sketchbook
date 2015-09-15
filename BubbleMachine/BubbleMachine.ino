/*
  Turn the bubble machine on for 60 seconds, off for 240.
 
  Based on the Blink example.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
// the bubble machine is also connected to pin 12
int bubbles = 12;
int i=0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(bubbles, OUTPUT);  
}

// the loop routine runs over and over again forever:
void loop() {
  // turn the LED and the bubble machine on
  digitalWrite(led, HIGH);
  digitalWrite(bubbles, HIGH);

  // wait for 60 seconds
  for(i=0; i<60; i++) {
    delay(1000);
  }

  // turn the LED and the bubble machine off  
  digitalWrite(led, LOW);
  digitalWrite(bubbles, LOW);
  
  // wait for 240 seconds (4 minutes)
  for(i=0; i<240; i++) {
    delay(1000);
  }
}
