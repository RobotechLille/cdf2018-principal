#ifndef __LCD_H_
#define __LCD_H_

#include "stdint.h"
#include "i2c.h"

#define LCD_ADDR 0x27

#define LCD_MODE_CHR 1
#define LCD_MODE_CMD 0

#define LCD_NB_LINES 2
#define LCD_NB_CHARS 16
#define LCD_NB_TOTAL LCD_NB_LINES*LCD_NB_CHARS


#define LCD_LINE_1 0x80 // 1st line
#define LCD_LINE_2 0xC0 // 2nd line

#define LCD_BACKLIGHT_ON 0x08
#define LCD_BACKLIGHT_OFF 0x08
#define LCD_BACKLIGHT LCD_BACKLIGHT_ON

#define LCD_MASK_ENABLE 0x04 // Enable bit

// Public
void initLCD();
void clearLCD();
void printToLCD(uint8_t line, char* s);
void printRightLCD(uint8_t line, char* s);
void printfToLCD(uint8_t line, char* s, ...);

// Private
void sendLCD(uint8_t bits, uint8_t mode);
void toggleEnableLCD(uint8_t bits);
void gotoLCD(uint8_t line);
void charLCD(char c);
void printLCD(char* s);
void displayLCD();
void lockLCD();
void unlockLCD();

#endif
