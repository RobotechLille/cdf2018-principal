#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "lcd.h"

int lcdFd;
pthread_mutex_t sLCD;

char virtual[LCD_NB_TOTAL];
uint8_t pos;

void initLCD()
{
    lcdFd = openI2C(LCD_ADDR);
    offLCD();
    onLCD();
    pinMode(LCD_ON_PIN, OUTPUT);
}

void eraseLCD()
{
    lockLCD();
    sendLCD(0x01, LCD_MODE_CMD);
    sendLCD(0x02, LCD_MODE_CMD);

    for (int i = 0; i < LCD_NB_TOTAL; i++) {
        virtual[i] = ' ';
    }
    unlockLCD();
}


void onLCD()
{
    digitalWrite(LCD_ON_PIN, LOW);
    delay(100);

    // TODO More details
    sendLCD(0x33, LCD_MODE_CMD); // Initialise
    sendLCD(0x32, LCD_MODE_CMD); // Initialise
    sendLCD(0x06, LCD_MODE_CMD); // Cursor move direction
    sendLCD(0x0C, LCD_MODE_CMD); // Blink Off
    sendLCD(0x28, LCD_MODE_CMD); // Data length, number of lines, font size
    eraseLCD();

    delay(50);
}

void offLCD()
{
    digitalWrite(LCD_ON_PIN, HIGH);
    delay(100);
}

void resetLCD()
{
    char line1bkp[LCD_NB_TOTAL];
    char line2bkp[LCD_NB_TOTAL];
    memcpy(line1bkp, virtual, LCD_NB_CHARS * sizeof(char));
    memcpy(line2bkp, virtual + LCD_NB_CHARS, LCD_NB_CHARS * sizeof(char));
    offLCD();
    onLCD();
    printToLCD(LCD_LINE_1, line1bkp);
    printToLCD(LCD_LINE_2, line2bkp);
}

void clearLCD()
{
    offLCD();
    onLCD();
}

void gotoLCD(uint8_t line)
{
    if (line >= LCD_LINE_2) {
        pos = 1 * LCD_NB_CHARS + line - LCD_LINE_2;
    } else if (line >= LCD_LINE_1) {
        pos = 0 * LCD_NB_CHARS + line - LCD_LINE_1;
    } else {
        pos = LCD_NB_TOTAL;
    }
    sendLCD(line, LCD_MODE_CMD);
}

void charLCD(char c)
{
    if (pos >= 0 && pos < LCD_NB_TOTAL) {
        virtual[pos] = c;
    }
    pos++;
    sendLCD(c, LCD_MODE_CHR);
}

void printLCD(char* s)
{
    while (*s != '\0') {
        charLCD(*s);
        s++;
    }
}

void displayLCD()
{
    printf("\n┌");
    for (int i = 0; i < LCD_NB_CHARS; i++) {
        printf("─");
    }
    printf("┐\n");
    for (int y = 0; y < LCD_NB_LINES; y++) {
        printf("│");
        for (int x = 0; x < LCD_NB_CHARS; x++) {
            putchar(virtual[y * LCD_NB_CHARS + x]);
        }
        printf("│\n");
    }
    printf("└");
    for (int i = 0; i < LCD_NB_CHARS; i++) {
        printf("─");
    }
    printf("┘\n");
}

void printToLCD(uint8_t line, char* s)
{
    lockLCD();
    gotoLCD(line);
    printLCD(s);
    displayLCD();
    unlockLCD();
}

void printRightLCD(uint8_t line, char* s)
{
    printToLCD(line + LCD_NB_CHARS - strlen(s), s);
}

void printfToLCD(uint8_t line, char* s, ...)
{
    char buffer[LCD_NB_TOTAL];
    va_list args;
    va_start(args, s);
    vsnprintf(buffer, LCD_NB_TOTAL, s, args);
    va_end(args);
    printToLCD(line, buffer);
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

void lockLCD()
{
    pthread_mutex_lock(&sLCD);
}

void unlockLCD()
{
    pthread_mutex_unlock(&sLCD);
}
