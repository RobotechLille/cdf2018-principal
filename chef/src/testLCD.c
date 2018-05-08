/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "i2c.h"
#include "lcd.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    wiringPiSetup();
    initI2C();
    initLCD();

    for (;;) {
        printToLCD(LCD_LINE_1, "Buongiorno");
        printToLCD(LCD_LINE_2, "Come stai?");
        sleep(1);
        resetLCD();
        sleep(1);
    }
}
