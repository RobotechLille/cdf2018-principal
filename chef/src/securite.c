#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "securite.h"

// Globales
pthread_t tSecurite;
pthread_mutex_t secPolling;
pthread_mutex_t secData;
struct F2CI_CAPTs secRaw;
float secFront, secBack;

void onF2CI_CAPT()
{
    readCF(&secRaw, sizeof(struct F2CI_CAPTs));
    pthread_mutex_unlock(&secPolling);
}

void* TaskSecurite(void* pData)
{
    (void)pData;

    for (;;) {
        pthread_mutex_lock(&secPolling);
        sendCF(F2CI_CAPT, NULL, 0);
        // Waiting for reception
        pthread_mutex_lock(&secPolling);
        pthread_mutex_unlock(&secPolling);

        pthread_mutex_lock(&secData);
        secFront = (float)secRaw.front * SOUND_MM_P_MS;
        secBack = (float)secRaw.back * SOUND_MM_P_MS;
        pthread_mutex_unlock(&secData);

        usleep(SENSOR_SAMPLING_INTERVAL * 1000);
    }

    return NULL;
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
    pthread_mutex_init(&secPolling, NULL);
    pthread_mutex_init(&secData, NULL);
    registerRxHandlerCF(F2CI_CODER, onF2CI_CAPT);
    registerDebugVar("secFront", ld, &secFront);
    registerDebugVar("secBack", ld, &secBack);
    pthread_create(&tSecurite, NULL, TaskSecurite, NULL);
}

void deconfigureSecurite()
{
    pthread_cancel(tSecurite);
}

