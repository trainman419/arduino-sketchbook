#include <SoftwareSerial.h>
#include <Adafruit_Thermal.h>
#include <DualVNH5019MotorShield.h>

#include "Lcd.h"


#define MENU_ITEMS 11

  // Menu:
  //  1. Start
  //  2. Max Psi
  //  3. Max hold time
  //  4. Step psi
  //  5. Step hold time
  //  6. Release time
  //  7. Loss threshold
  //  8. Manual fill
  //  9. Manual bleed
  //  10. Manual pump
  //  11. Self test
enum MenuSelection : uint8_t {
  START = 0,
  MAX = 1,
  MAX_HOLD = 2,
  STEP = 3,
  STEP_HOLD = 4,
  RELEASE = 5,
  LOSS_THRESH = 6,
  MANUAL_FILL = 7,
  MANUAL_BLEED = 8,
  MANUAL_PUMP = 9,
  SELF_TEST = 10
};

SoftwareSerial LCD_port(20, 4);
LCD lcd(LCD_port);

SoftwareSerial printerPort(18, 17);
Adafruit_Thermal printer(&printerPort);

double step_psi = 10.0f;
uint8_t step_hold_minutes = 5;
double loss_threshold_psi = 1.0f;
double test_pressure_psi = 100.0f;
uint16_t release_time_minutes = 60;

// Encoder handling
#define CLK_PIN 3
#define DT_PIN 2
#define BUTTON_PIN 7

volatile int8_t counter = 0;
volatile unsigned long last_time = 0;  // for debouncing
unsigned long t = 0;

void ISR_encoderChange() {
  if ((millis() - last_time) < 50)  // debounce time is 50ms
    return;

  if (digitalRead(DT_PIN) == HIGH) {
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    counter--;
  } else {
    // the encoder is rotating in clockwise direction => increase the counter
    counter++;
  }

  last_time = millis();
}

void menuLine(MenuSelection m, bool selected, bool active) {
  constexpr char activeChar = '*';
  constexpr char selectChar = '>';
  constexpr char idleChar = ' ';
  constexpr int bufferSize = 16;
  char buffer[bufferSize+1];
  const int8_t idx = (int8_t)m;
  int p = 0;

  // Menu:
  //  1. Start
  //  2. Max Psi
  //  3. Max hold time
  //  4. Step psi
  //  5. Step hold time
  //  6. Release time
  //  7. Loss threshold
  //  8. Manual fill
  //  9. Manual bleed
  //  10. Manual pump
  //  11. Self test
  // buffer[0] = (active?activeChar:(selected?selectChar:idleChar));
  // p = 1;
  p = snprintf(buffer, bufferSize,
    "%c%2d. ",
    (active?activeChar:(selected?selectChar:idleChar)),
    idx+1);

  switch (m) {
    case MenuSelection::START:
      p += snprintf(buffer+p, bufferSize-p, "Start");
      break;
    case MenuSelection::MAX:
      p += snprintf(buffer+p, bufferSize-p, "Max %3d",
      (int)test_pressure_psi);
      break;
    case MenuSelection::MAX_HOLD:
      p += snprintf(buffer+p, bufferSize-p, "Max hold %3d",
      (int)test_pressure_psi);
      break;
    case MenuSelection::STEP:
      p += snprintf(buffer+p, bufferSize-p, "Step %2d",
      (int)step_psi);
      break;
    case MenuSelection::STEP_HOLD:
      p += snprintf(buffer+p, bufferSize-p, "Step hold %2d",
      step_hold_minutes);
      break;
    case MenuSelection::RELEASE:
      p += snprintf(buffer+p, bufferSize-p, "Release %3d",
      release_time_minutes);
      break;
    default:
      break;
  }
  // Fill remainder of buffer with spaces to clear display.
  for (; p<bufferSize; p++) {
    buffer[p] = ' ';
  }

  digitalWrite(13, HIGH);
  lcd.write(buffer);
  digitalWrite(13, LOW);
}

void updateDisplay() {
  static int8_t last_counter = 0;
  static int8_t last_button = 0;
  static MenuSelection selection = MenuSelection::START;
  bool active = false;

  int8_t button = !digitalRead(BUTTON_PIN);
  // Snapshot selection state so it doesn't change while we're drawing the display.
  // const MenuSelection selection = ::selection;

  // Update menu selection.
  int8_t counter_delta = counter - last_counter;
  last_counter = counter;

  if (button) {
    if (button != last_button) {
      active = !active;
    }
  }
  last_button = button;

  if (active) {
    switch (selection) {
      case MenuSelection::STEP:
        step_psi += counter_delta;
        break;
      default:
        break;
    }
  } else {
    selection = static_cast<MenuSelection>((selection + counter_delta) % MENU_ITEMS);
  }

  digitalWrite(13, HIGH);
  lcd.home();
  lcd.setCursor(0, 0);
  digitalWrite(13, LOW);

  if (selection < (MENU_ITEMS-1)) {
    menuLine(selection, true, active);
    menuLine(static_cast<MenuSelection>(selection+1), false, false);
  } else {
    menuLine(static_cast<MenuSelection>(selection-1), false, false);
    menuLine(selection, true, active);
  }
}

void setup() {
  pinMode(13, OUTPUT); // LED pin as output.

  // Configure encoder pins as inputs.
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Set up encoder interrupt.
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderChange, RISING);

  lcd.begin();
  updateDisplay();

  printerPort.begin(9600);
  printer.begin();

  // Works, but don't use all the paper ;)
  //printer.println("Hello World");

  // Print the 75x75 pixel logo in adalogo.h:
  //printer.printBitmap(adalogo_width, adalogo_height, adalogo_data);
  t = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(25);
  digitalWrite(13, LOW);

  updateDisplay();

  t += 50;
  unsigned long dt = t - millis();
  if (dt < 25) {
    delay(dt);
  }
}
