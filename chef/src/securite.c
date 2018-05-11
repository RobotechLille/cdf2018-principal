#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "fpga.h"
#include "securite.h"

// Globales
pthread_t tSecurite;
pthread_mutex_t secData;
float secFrontL, secFrontR, secBackL, secBackR;
float secFront, secBack;

void* TaskSecurite(void* pData)
{
    (void)pData;

    for (;;) {

        pthread_mutex_lock(&secData);
        uint16_t secFrontLB = (readI2C(fdFPGA(), CAPT_FRONT_LEFT_H) << 8 | readI2C(fdFPGA(), CAPT_FRONT_LEFT_L));
        uint16_t secFrontRB = (readI2C(fdFPGA(), CAPT_FRONT_RIGHT_H) << 8 | readI2C(fdFPGA(), CAPT_FRONT_RIGHT_L));
        uint16_t secBackLB = (readI2C(fdFPGA(), CAPT_BACK_LEFT_H) << 8 | readI2C(fdFPGA(), CAPT_BACK_LEFT_L));
        uint16_t secBackRB = (readI2C(fdFPGA(), CAPT_BACK_RIGHT_H) << 8 | readI2C(fdFPGA(), CAPT_BACK_RIGHT_L));
        secFrontL = secFrontLB * SOUND_MM_P_MS;
        secFrontR = secFrontRB * SOUND_MM_P_MS;
        secBackL = secBackLB * SOUND_MM_P_MS;
        secBackR = secBackRB * SOUND_MM_P_MS;
        secFront = fmin(secFrontL, secFrontR);
        secBack = fmin(secBackL, secBackR);
        pthread_mutex_unlock(&secData);

        usleep(SENSOR_SAMPLING_INTERVAL * 1000);
    }

    return NULL;
}

void getAllDistance(float* frontL, float* frontR, float* backL, float* backR)
{
    pthread_mutex_lock(&secData);
    *frontR = secFrontR;
    *frontL = secFrontL;
    *backL = secBackL;
    *backR = secBackR;
    pthread_mutex_unlock(&secData);
}

void getDistance(float* front, float* back)
{
    pthread_mutex_lock(&secData);
    *front = secFront;
    *back = secBack;
    pthread_mutex_unlock(&secData);
}

void configureSecurite()
{
    registerDebugVar("secFront", f, &secFront);
    registerDebugVar("secBack", f, &secBack);
    registerDebugVar("secFrontL", f, &secFrontL);
    registerDebugVar("secBackL", f, &secBackL);
    registerDebugVar("secFrontR", f, &secFrontR);
    registerDebugVar("secBackR", f, &secBackR);
    pthread_mutex_init(&secData, NULL);
    pthread_create(&tSecurite, NULL, TaskSecurite, NULL);
}

void deconfigureSecurite()
{
    pthread_cancel(tSecurite);
}
