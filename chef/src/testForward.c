/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "lcd.h"
#include "motor.h"
#include "buttons.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    float patate = 2;
    if (argc == 2) {
        sscanf(argv[1], "%f", &patate);
    }

    wiringPiSetup();

    initI2C();
    initLCD();
    configureButtons();
    configureMotor();

    for (;;) {
        clearLCD();
        printToLCD(LCD_LINE_1, "Forward");
        setMoteurTension(patate, patate);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Right");
        setMoteurTension(-patate, patate);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Left");
        setMoteurTension(patate, -patate);
        pressedButton(BUT_BLOCK);

        clearLCD();
        printToLCD(LCD_LINE_1, "Backward");
        setMoteurTension(-patate, -patate);
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
