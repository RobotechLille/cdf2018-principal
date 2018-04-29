/*
 * Fonctions de calcul de la position du robot
 */

#include <stdio.h>

#include "debug.h"
#include "position.h"

// Globales
struct position actuel;
struct F2CI_CODERs deltaCoders;
pthread_mutex_t posPolling;
pthread_t tPosition;

// Globales
unsigned int nbCalcPos;
long lCodTot, rCodTot;


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
    registerRxHandler(F2CI_CODER, onF2CI_CODER);
    registerDebugVar("lCodTot", ld, &lCodTot);
    registerDebugVar("rCodTot", ld, &rCodTot);
    registerDebugVar("nbCalcPos", d, &nbCalcPos);
    pthread_create(&tPosition, NULL, TaskPosition, NULL);
}

void deconfigurePosition()
{
    pthread_cancel(tPosition);
}
