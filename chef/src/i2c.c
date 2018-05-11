#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPiI2C.h>

#include "i2c.h"

pthread_mutex_t sI2C;

bool i2cInited = false;

void initI2C()
{
    if (!i2cInited) {
        pthread_mutex_init(&sI2C, NULL);
        i2cInited = true;
    }
}

int openI2C(uint8_t address)
{
    lockI2C();
    int fd = wiringPiI2CSetup(address);
    unlockI2C();
    if (fd < 0) {
        perror("wiringPiI2CSetup");
        exit(EXIT_FAILURE);
    }
    return fd;
}

uint8_t readI2C(int fd, uint8_t reg)
{
    lockI2C();
    int res;
    int delay = 1;
    static char errBuffer[1024];
    for (int i = 0; i < I2C_DRIVEN_HIGH_RETRIES; i++) {
        while ((res = wiringPiI2CReadReg8(fd, reg)) < 0) {
            snprintf(errBuffer, 1024, "wiringPiI2CReadReg8 @%3d %2x %9d", fd, reg, delay);
            perror(errBuffer);
            usleep(delay);
            delay *= 2;
        }
        if (res != 0xFF) {
            break;
        }
    }
    unlockI2C();
    return res;
}

void writeI2C(int fd, uint8_t reg, uint8_t data)
{
    lockI2C();
    int delay = 1;
    static char errBuffer[1024];
    while (wiringPiI2CWriteReg8(fd, reg, data) < 0) {
        snprintf(errBuffer, 1024, "wiringPiI2CWriteReg8 @%3d %2x←%2x %9d", fd, reg, data, delay);
        perror(errBuffer);
        usleep(delay);
        delay *= 2;
    }
    unlockI2C();
}

void lockI2C()
{
    pthread_mutex_lock(&sI2C);
}

void unlockI2C()
{
    pthread_mutex_unlock(&sI2C);
}
