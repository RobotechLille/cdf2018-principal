/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "lcd.h"
#include "CF.h"
#include "movement.h"
#include "buttons.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    wiringPiSetup();

    initI2C();
    initLCD();
    configureCF();
    configureButtons();
    configureMovement();

    for (;;) {
        clearLCD();
        printToLCD(LCD_LINE_1, "Forward");
        changerMoteurs(3.3, 3.3);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Right");
        changerMoteurs(-3.3, 3.3);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Left");
        changerMoteurs(-3.3, -3.3);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Backward");
        changerMoteurs(3.3, -3.3);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Brake");
        brake();
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Free");
        freewheel();
        pressedButton(BUT_BLOCK);
    }

}