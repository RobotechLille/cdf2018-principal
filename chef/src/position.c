/*
 * Fonctions de calcul de la position du robot
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "dimensions.h"
#include "position.h"

// Globales
struct position connu;
struct F2CI_CODERs deltaCoders;
pthread_mutex_t posPolling;
pthread_mutex_t posConnu;
pthread_cond_t newPos;
pthread_t tPosition;

// Globales
unsigned int nbCalcPos;
long lCodTot, rCodTot;

void* TaskPosition(void* pData)
{
    (void)pData;

    resetPosition();
    nbCalcPos = 0;
    lCodTot = 0;
    rCodTot = 0;

    for (;;) {

        // Sending
        pthread_mutex_lock(&posPolling);
        sendCF(F2CI_CODER, NULL, 0);

        // Waiting for reception
        pthread_mutex_lock(&posPolling);
        pthread_mutex_unlock(&posPolling);

        // Calculation
#ifdef INVERSE_L_CODER
        deltaCoders.dL = -deltaCoders.dL;
#endif
#ifdef INVERSE_R_CODER
        deltaCoders.dR = -deltaCoders.dR;
#endif
        lCodTot += deltaCoders.dL;
        rCodTot += deltaCoders.dR;

        float dR = deltaCoders.dR * AV_PER_CYCLE;
        float dL = deltaCoders.dL * AV_PER_CYCLE;

        float deltaO = atan2(dR - dL, DISTANCE_BETWEEN_WHEELS);
        float deltaD = (dL + dR) / 2;

        pthread_mutex_lock(&posConnu);
        connu.o += deltaO;
        float deltaX = deltaD * cos(connu.o);
        float deltaY = deltaD * sin(connu.o);
        connu.x += deltaX;
        connu.y += deltaY;
        nbCalcPos++;
        pthread_cond_signal(&newPos);
        pthread_mutex_unlock(&posConnu);


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
    pthread_mutex_init(&posPolling, NULL);
    pthread_mutex_init(&posConnu, NULL);
    pthread_cond_init(&newPos, NULL);
    registerRxHandler(F2CI_CODER, onF2CI_CODER);
    registerDebugVar("lCodTot", ld, &lCodTot);
    registerDebugVar("rCodTot", ld, &rCodTot);
    connu.x = 0;
    connu.y = 0;
    connu.o = 0;
    registerDebugVar("xConnu", f, &connu.x);
    registerDebugVar("yConnu", f, &connu.y);
    registerDebugVar("oConnu", f, &connu.o);
    registerDebugVar("nbCalcPos", d, &nbCalcPos);
    pthread_create(&tPosition, NULL, TaskPosition, NULL);
}

void deconfigurePosition()
{
    pthread_cancel(tPosition);
}

void getCoders(long* l, long* r)
{
    *l = lCodTot;
    *r = rCodTot;
}

unsigned int getPosition(struct position* pos)
{
    pthread_mutex_lock(&posConnu);
    unsigned int nb = nbCalcPos;
    memcpy(pos, &connu, sizeof(struct position));
    pthread_mutex_unlock(&posConnu);
    return nb;
}

unsigned int getPositionNewer(struct position* pos, unsigned int lastCalc)
{
    pthread_mutex_lock(&posConnu);
    while (nbCalcPos <= lastCalc) {
        pthread_cond_wait(&newPos, &posConnu);
    }
    pthread_mutex_unlock(&posConnu);
    return getPosition(pos);
}

void setPosition(struct position* pos)
{
    pthread_mutex_lock(&posConnu);
    memcpy(&connu, pos, sizeof(struct position));
    pthread_mutex_unlock(&posConnu);
}

void resetPosition()
{
    struct position pos = {0, 0, 0};
    setPosition(&pos);
}
