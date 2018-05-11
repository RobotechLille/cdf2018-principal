#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "actionneurs.h"
#include "buttons.h"
#include "debug.h"
#include "i2c.h"
#include "lcd.h"
#include "motor.h"
#include "movement.h"
#include "position.h"

int main()
{
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "Impossible d'initialiser WiringPi\n");
        exit(EXIT_FAILURE);
    }
    initI2C();
    srand(time(NULL));

    configureDebug();
    configurePosition();
    configureMovement();
    configureButtons();
    initLCD();
    debugSetActive(true);
    startDebug();

    printToLCD(LCD_LINE_1, "RGE: Set origin");
    printToLCD(LCD_LINE_2, "JNE: Toggle free");

    bool isFree = true;
    disableAsservissement();
    freewheel();

    for (;;) {
        switch (pressedButton(BUT_BLOCK)) {
        case jaune:
            isFree = !isFree;
            if (isFree) {
                disableAsservissement();
                freewheel();
            } else {
                enableAsservissement();
            }
            break;
        case rouge:
            resetPosition();
            break;
        default:
            break;
        }
        clearLCD();
        if (isFree) {
            printToLCD(LCD_LINE_1, "Freewheel");
        } else {
            printToLCD(LCD_LINE_1, "Asservi");
        }
    }

    deconfigureMovement();
    deconfigurePosition();
    deconfigureDebug();
    return EXIT_SUCCESS;
}
