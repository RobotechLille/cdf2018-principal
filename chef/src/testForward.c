/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPiI2C.h>

#include "lcd.h"
#include "movement.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    initI2C();

    initLCD();
    clearLCD();
    printToLCD(LCD_LINE_1, "Forward");

    configureMovement();

    for (;;) {
        changerMoteurs(1.5, 2);
        sleep(1);
    }

}
