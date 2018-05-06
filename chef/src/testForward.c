/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "lcd.h"
#include "CF.h"
#include "motor.h"
#include "buttons.h"

#define PATATE 2

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    wiringPiSetup();

    initI2C();
    initLCD();
    configureCF();
    configureButtons();

    for (;;) {
        clearLCD();
        printToLCD(LCD_LINE_1, "Forward");
        setMoteurTension(PATATE, PATATE);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Right");
        setMoteurTension(-PATATE, PATATE);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Left");
        setMoteurTension(PATATE, -PATATE);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Backward");
        setMoteurTension(-PATATE, -PATATE);
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
