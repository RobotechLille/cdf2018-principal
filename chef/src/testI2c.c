/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPiI2C.h>

#include "i2c.h"
#include "lcd.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    initI2C();
    initLCD();
    clearLCD();
    gotoLCD(LCD_LINE_1);
    printLCD("Bojour");

}
