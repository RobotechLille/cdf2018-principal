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

    struct gyroRaw a;
    struct gyroRaw t;
    for (;;) {
        a = readGyroRaw();
        t.x += a.x;
        t.y += a.y;
        t.z += a.z;
        usleep(100*1000);
        printf("X:%5d Y:%5d Z:%5d\n", t.x, t.y, t.z);
    }

    exit(0);
}
