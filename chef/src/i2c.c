#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
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
    uint8_t res = wiringPiI2CReadReg8(fd, reg);
    unlockI2C();
    return res;
}

void writeI2C(int fd, uint8_t reg, uint8_t data)
{
    lockI2C();
    int delay = 1;
    while (wiringPiI2CWriteReg8(fd, reg, data) < 0) {
        perror("wiringPiI2CWriteReg8");
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
