#include <SoftwareSerial.h>
#include <Adafruit_Thermal.h>
#include <SerLCD.h>

SoftwareSerial LCD_port(20, 4);
SerLCD lcd;

SoftwareSerial printerPort(18, 17);
Adafruit_Thermal printer(&printerPort);


int iter = 0;

void updateDisplay() {
  char buffer[20];
  lcd.home();
  lcd.setCursor(0, 0);
  snprintf(buffer, 20, "Hello World % 4d", iter);
  lcd.print(buffer);
  snprintf(buffer, 20, "                ");
  lcd.print(buffer);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
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
  iter++;
  updateDisplay();
}
