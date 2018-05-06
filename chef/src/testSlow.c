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

#define VIT 0.40


void setPWMTensionWrapper(float l, float r) {
    /* clearLCD(); */
    printfToLCD(LCD_LINE_1, "L: %f", l);
    printfToLCD(LCD_LINE_2, "R: %f", r);
    setPWMTension(l, r);
}

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

    setPWMTensionWrapper(VIT, VIT);

    for (;;) {
        setPWMTensionWrapper(VIT, VIT);
        pressedButton(BUT_BLOCK);
        brake();
        pressedButton(BUT_BLOCK);
    }

}
