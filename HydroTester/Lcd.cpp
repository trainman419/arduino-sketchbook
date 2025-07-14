/*
 * Serial-optimized LCD library.
 * Derived from https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
 *
 * Modified by: Austin Hendrix 
 */
#include "Lcd.h"

/*
 * Initialize the display
 *
 */
void LCD::begin()
{
  transmit(SPECIAL_COMMAND);                      //Send special command character
  transmit(LCD_DISPLAYCONTROL | _displayControl); //Send the display command
  transmit(SPECIAL_COMMAND);                      //Send special command character
  transmit(LCD_ENTRYMODESET | _displayMode);      //Send the entry mode command
  transmit(SETTING_COMMAND);                      //Put LCD into setting mode
  transmit(CLEAR_COMMAND);                        //Send clear display command
  // delay(50);                                      //let things settle a bit
} //init

/*
  * Send a command to the display.
  * Used by other functions.
  *
  * byte command to send
  */
void LCD::command(byte command)
{
  transmit(SETTING_COMMAND); //Put LCD into setting mode
  transmit(command);         //Send the command code

  //delay(10); //Hang out for a bit
}

/*
 * Send a special command to the display.  Used by other functions.
 *
 * byte command to send
 */
void LCD::specialCommand(byte command)
{
  transmit(SPECIAL_COMMAND); //Send special command character
  transmit(command);         //Send the command code

  //delay(50); //Wait a bit longer for special display commands
}

/*
 * Send multiple special commands to the display.
 * Used by other functions.
 *
 * byte command to send
 * byte count number of times to send
 */
void LCD::specialCommand(byte command, byte count)
{
  for (int i = 0; i < count; i++)
  {
    transmit(SPECIAL_COMMAND); //Send special command character
    transmit(command);         //Send command code
  }                            // for
  //delay(50); //Wait a bit longer for special display commands
}

/*
 * Send the clear command to the display.  This clears the
 * display and forces the cursor to return to the beginning
 * of the display.
 */
void LCD::clear()
{
  command(CLEAR_COMMAND);
  delay(10); // a little extra delay after clear
}
/*
 * Send the home command to the display.  This returns the cursor
 * to return to the beginning of the display, without clearing
 * the display.
 */
void LCD::home()
{
  specialCommand(LCD_RETURNHOME);
}

/*
 * Set the cursor position to a particular column and row.
 *
 * column - byte 0 to 19
 * row - byte 0 to 3
 *
 */
void LCD::setCursor(byte col, byte row)
{
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};

  //kepp variables in bounds
  row = min(row, (byte)(MAX_ROWS - 1)); //row cannot be greater than max rows

  //send the command
  specialCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
} // setCursor

/*
 * Write a byte to the display.
 * Required for Print.
 */
size_t LCD::write(uint8_t b)
{
  transmit(b);
  //delay(10);         // wait a bit
  return 1;
} // write

/*
 * Write a character buffer to the display.
 * Required for Print.
 */
size_t LCD::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--)
  {
    transmit(*buffer++);
    n++;
  }                  //while
  //delay(10);         //
  return n;
} //write

/*
 * Write a string to the display.
 * Required for Print.
 */
size_t LCD::write(const char *str)
{
  if (str == NULL)
    return 0;
  return write((const uint8_t *)str, strlen(str));
}