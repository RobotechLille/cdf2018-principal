/*
 * Fonctions de calcul de la position du robot
 */

#include "position.h"
#include <stdio.h>

void* TaskPosition(void* pData)
{
    (void)pData;

    nbCalcPos = 0;
    lCodTot = 0;
    rCodTot = 0;

    pthread_mutex_init(&posPolling, NULL);

    for (;;) {

        // Sending
        pthread_mutex_lock(&posPolling);
        sendCF(F2CI_CODER, NULL, 0);

        // Waiting for reception
        pthread_mutex_lock(&posPolling);
        pthread_mutex_unlock(&posPolling);

        // Calculation
        nbCalcPos++;
        lCodTot += deltaCoders.dL;
        rCodTot += deltaCoders.dR;

    }

    return NULL;
}

void onF2CI_CODER()
{
    readCF(&deltaCoders, sizeof(struct F2CI_CODERs));
    pthread_mutex_unlock(&posPolling);
}

void configurePosition()
{
    pthread_create(&tPosition, NULL, TaskPosition, NULL);
    registerRxHandler(F2CI_CODER, onF2CI_CODER);
}

void deconfigurePosition()
{
    pthread_cancel(tPosition);
}
