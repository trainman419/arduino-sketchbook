/*
 * Serial-optimized LCD library.
 * Derived from https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library
 *
 * Modified by: Austin Hendrix 
 */

#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

#define DISPLAY_ADDRESS1 0x72 //This is the default address of the OpenLCD
#define MAX_ROWS 4
#define MAX_COLUMNS 20

//OpenLCD command characters
#define SPECIAL_COMMAND 254  //Magic number for sending a special command
#define SETTING_COMMAND 0x7C //124, |, the pipe character: The command to change settings: baud, lines, width, backlight, splash, etc

//OpenLCD commands
#define CLEAR_COMMAND 0x2D					//45, -, the dash character: command to clear and home the display
#define CONTRAST_COMMAND 0x18				//Command to change the contrast setting
#define ADDRESS_COMMAND 0x19				//Command to change the i2c address
#define SET_RGB_COMMAND 0x2B				//43, +, the plus character: command to set backlight RGB value
#define ENABLE_SYSTEM_MESSAGE_DISPLAY 0x2E  //46, ., command to enable system messages being displayed
#define DISABLE_SYSTEM_MESSAGE_DISPLAY 0x2F //47, /, command to disable system messages being displayed
#define ENABLE_SPLASH_DISPLAY 0x30			//48, 0, command to enable splash screen at power on
#define DISABLE_SPLASH_DISPLAY 0x31			//49, 1, command to disable splash screen at power on
#define SAVE_CURRENT_DISPLAY_AS_SPLASH 0x0A //10, Ctrl+j, command to save current text on display as splash

// special commands
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

class LCD /* : public Print */ {
  public:
    LCD(SoftwareSerial &port) : _serialPort(port) {}

    /*
     * Initialize the display
     *
     */
    void begin();

    /*
      * Send a command to the display.
      * Used by other functions.
      *
      * byte command to send
      */
    void command(byte command);

    /*
     * Send a special command to the display.  Used by other functions.
     *
     * byte command to send
     */
    void specialCommand(byte command);

    /*
     * Send multiple special commands to the display.
     * Used by other functions.
     *
     * byte command to send
     * byte count number of times to send
     */
    // void specialCommand(byte command, byte count);

    /*
    * Send the clear command to the display.  This clears the
    * display and forces the cursor to return to the beginning
    * of the display.
    */
    void clear();

    /*
     * Send the home command to the display.  This returns the cursor
     * to return to the beginning of the display, without clearing
     * the display.
     */
    void home();

    /*
     * Set the cursor position to a particular column and row.
     *
     * column - byte 0 to 19
     * row - byte 0 to 3
     *
     */
    void setCursor(byte col, byte row);

    /* Overrides for Print */
    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *str);
  private:

    /*
     * Send data to the device
     *
     * data - byte to send
     */
    inline void transmit(uint8_t data)
    {
      _serialPort.write(data);
    }
    
    SoftwareSerial &_serialPort;
    byte _displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	  byte _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

};