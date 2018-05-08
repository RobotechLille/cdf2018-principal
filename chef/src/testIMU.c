/* Teste l'IMU */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPiI2C.h>

#include "i2c.h"
#include "imu.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    initI2C();
    configureIMU();
    if (!connectedIMU()) {
        printf("IMU not connected\n");
        exit(1);
    }

    struct axesRaw a;
    struct axesRaw g;
    for (;;) {
        a = readAcelRaw();
        printf("X:%5d Y:%5d Z:%5d\n", a.x, a.y, a.z);
        g = readGyroRaw();
        printf("X:%5d Y:%5d Z:%5d\n", g.x, g.y, g.z);
        printf("\n");
        usleep(100*1000);
    }

    exit(0);
}
