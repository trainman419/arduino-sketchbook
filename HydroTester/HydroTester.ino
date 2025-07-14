#include <SoftwareSerial.h>
#include <Adafruit_Thermal.h>
#include <SerLCD.h>
#include <DualVNH5019MotorShield.h>


SoftwareSerial LCD_port(20, 4);
SerLCD lcd;

SoftwareSerial printerPort(18, 17);
Adafruit_Thermal printer(&printerPort);

double step_psi = 10.0f;
uint8_t step_hold_minutes = 5;
double loss_threshold_psi = 1.0f;
double test_pressure_psi = 100.0f;
uint16_t release_time_minutes = 60;

// Other UI options.
//  Manual fill
//  Manual valve and pump actuation
//  Self test - ramp to max pressure, hold. Built-in pressure loss threshold.

// Encoder handling
#define CLK_PIN 3
#define DT_PIN 2
#define DIRECTION_CW 0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

volatile int8_t counter = 0;
volatile unsigned long last_time;  // for debouncing

enum class MenuSelection {
  STEP,
  HOLD,
  MAX,
  RELEASE
};

MenuSelection selection = MenuSelection::STEP;

void incrementSelection() {
    switch (selection) {
      case MenuSelection::STEP:
        selection = MenuSelection::HOLD;
        break;
      case MenuSelection::HOLD:
        selection = MenuSelection::MAX;
        break;
      case MenuSelection::MAX:
        selection = MenuSelection::RELEASE;
        break;
      case MenuSelection::RELEASE:
        selection = MenuSelection::STEP;
        break;
    }
}

void decrementSelection() {
      switch (selection) {
      case MenuSelection::STEP:
        selection = MenuSelection::RELEASE;
        break;
      case MenuSelection::HOLD:
        selection = MenuSelection::STEP;
        break;
      case MenuSelection::MAX:
        selection = MenuSelection::HOLD;
        break;
      case MenuSelection::RELEASE:
        selection = MenuSelection::MAX;
        break;
    }
}

void ISR_encoderChange() {
  if ((millis() - last_time) < 50)  // debounce time is 50ms
    return;

  if (digitalRead(DT_PIN) == HIGH) {
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    counter--;
    decrementSelection();
  } else {
    // the encoder is rotating in clockwise direction => increase the counter
    counter++;
    incrementSelection();
  }

  last_time = millis();
}

void updateDisplay() {
  char buffer[20];
  lcd.home();
  lcd.setCursor(0, 0);
  snprintf(buffer, 17, "%cStep % 2d %cHold % 2d", 
    (selection==MenuSelection::STEP)?'*':' ',
    (int)step_psi,
    (selection==MenuSelection::HOLD)?'*':' ',
    step_hold_minutes);
  lcd.print(buffer);
  snprintf(buffer, 17, "%cMax % 3d %cRelease % 3d",
    (selection==MenuSelection::MAX)?'*':' ',
    (int)test_pressure_psi,
    (selection==MenuSelection::RELEASE)?'*':' ',
    release_time_minutes);
  lcd.print(buffer);
}

void setup() {
  pinMode(13, OUTPUT); // LED pin as output.

  // Configure encoder pins as inputs.
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  // Set up encoder interrupt.
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderChange, RISING);


  LCD_port.begin(9600);

  lcd.begin(LCD_port);
  lcd.command(0x10);
  LCD_port.begin(38400);
  updateDisplay();

  printerPort.begin(9600);
  printer.begin();

  // Works, but don't use all the paper ;)
  //printer.println("Hello World");

  // Print the 75x75 pixel logo in adalogo.h:
  //printer.printBitmap(adalogo_width, adalogo_height, adalogo_data);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, 0);
  delay(500);
  digitalWrite(13, 1);
  delay(500);
  updateDisplay();
}
