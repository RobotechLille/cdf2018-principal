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

#define UP_TIME 1000
#define HIGH_TIME 3000
#define DOWN_TIME 1000
#define LOW_TIME 2000
#define MAXI 12
#define INTERVAL 10

void changerMoteursWrapper(float l, float r) {
    /* clearLCD(); */
    printfToLCD(LCD_LINE_1, "L: %f", l);
    printfToLCD(LCD_LINE_2, "R: %f", r);
    setMoteurTension(l, r);
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
    configureMotor();

    for (;;) {
        for (int i = 0; i < UP_TIME; i += INTERVAL) {
            float p = (float)i / (float)UP_TIME;
            changerMoteursWrapper(p * MAXI, p * MAXI);
            delay(INTERVAL);
        }
        changerMoteursWrapper(MAXI, MAXI);
        delay(HIGH_TIME);

        for (int i = 0; i < DOWN_TIME; i += INTERVAL) {
            float p = (float)i / (float)DOWN_TIME;
            p = 1 - p;
            changerMoteursWrapper(p * MAXI, p * MAXI);
            delay(INTERVAL);
        }
        changerMoteursWrapper(0, 0);
        delay(LOW_TIME);
        for (int i = 0; i < UP_TIME; i += INTERVAL) {
            float p = (float)i / (float)UP_TIME;
            changerMoteursWrapper(-p * MAXI, -p * MAXI);
            delay(INTERVAL);
        }
        changerMoteursWrapper(-MAXI, -MAXI);
        delay(HIGH_TIME);

        for (int i = 0; i < DOWN_TIME; i += INTERVAL) {
            float p = (float)i / (float)DOWN_TIME;
            p = 1 - p;
            changerMoteursWrapper(-p * MAXI, -p * MAXI);
            delay(INTERVAL);
        }
        changerMoteursWrapper(0, 0);
        delay(LOW_TIME);
    }

}
