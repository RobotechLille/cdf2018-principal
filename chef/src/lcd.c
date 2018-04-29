#include "lcd.h"
#include "i2c.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>

int lcdFd;

void initLCD()
{
    lcdFd = openI2C(LCD_ADDR);

    // TODO More details
    sendLCD(0x33, LCD_MODE_CMD); // Initialise
    sendLCD(0x32, LCD_MODE_CMD); // Initialise
    sendLCD(0x06, LCD_MODE_CMD); // Cursor move direction
    sendLCD(0x0C, LCD_MODE_CMD); // Blink Off
    sendLCD(0x28, LCD_MODE_CMD); // Data length, number of lines, font size
    sendLCD(0x01, LCD_MODE_CMD); // Clear display
    delayMicroseconds(500);
}

void clearLCD()
{
    sendLCD(0x01, LCD_MODE_CMD);
    sendLCD(0x02, LCD_MODE_CMD);
}

void gotoLCD(uint8_t line)
{
    sendLCD(line, LCD_MODE_CMD);
}

void charLCD(char c)
{
    sendLCD(c, LCD_MODE_CHR);
}

void printLCD(char* s)
{
    while (*s != '\0') {
        charLCD(*s);
        s++;
    }
}


void sendLCD(uint8_t bits, uint8_t mode)
{
    lockI2C();
    // High bits
    uint8_t part = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    wiringPiI2CReadReg8(lcdFd, part);
    toggleEnableLCD(part);

    // Low bits
    part = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;
    wiringPiI2CReadReg8(lcdFd, part);
    toggleEnableLCD(part);
    unlockI2C();
}

void toggleEnableLCD(uint8_t bits)
{
    wiringPiI2CReadReg8(lcdFd, (bits | LCD_MASK_ENABLE));
    delayMicroseconds(1);
    wiringPiI2CReadReg8(lcdFd, (bits & ~LCD_MASK_ENABLE));
    delayMicroseconds(50);
}
