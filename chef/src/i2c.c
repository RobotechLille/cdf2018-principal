#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiI2C.h>

#include "i2c.h"

pthread_mutex_t sI2C;

void initI2C()
{
    pthread_mutex_init(&sI2C, NULL);
}

int openI2C(int8_t address)
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

int8_t readI2C(int fd, int8_t reg)
{
    lockI2C();
    int8_t res = wiringPiI2CReadReg8(fd, reg);
    unlockI2C();
    return res;
}

void writeI2C(int fd, int8_t reg, int8_t data)
{

    lockI2C();
    int res = wiringPiI2CWriteReg8(fd, reg, data);
    unlockI2C();
    if (res < 0) {
        perror("wiringPiI2CWriteReg8");
        exit(EXIT_FAILURE);
    }
}

void lockI2C()
{
    pthread_mutex_lock(&sI2C);
}

void unlockI2C()
{
    pthread_mutex_unlock(&sI2C);
}
