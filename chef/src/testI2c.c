/* Teste si une broche est connecté à une autre */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPiI2C.h>

#include "srf08.h"
#include "i2c.h"
#include "lcd.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    initI2C();

    // Adresse du SRF05
    uint8_t address = atoi(argv[1]);

    printf("Connecting to %d (0x%x)\n", address, address);

    initLCD();
    clearLCD();
    gotoLCD(LCD_LINE_1);
    printLCD("Distance");

    int front = openSRF08(address);

    char line[16];
    float l = 0;
    for (;;) {
        startSRF08(front);

        gotoLCD(LCD_LINE_2);
        printf("l = %3f\n", l);
        sprintf(line, "%f mm", l);
        printLCD(line);
        printLCD("        ");

        waitSRF08(front);
        l = getSRF08(front);

    }


}
