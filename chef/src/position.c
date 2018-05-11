/*
 * Fonctions de calcul de la position du robot
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "dimensions.h"
#include "fpga.h"
#include "position.h"
#include "common.h"

// Globales
struct position connu;
pthread_mutex_t posConnu;
pthread_cond_t newPos;
pthread_t tPosition;

// Globales
unsigned int nbCalcPos;
long lCodTot, rCodTot;

uint16_t oldL, oldR;
uint16_t newL, newR;
int16_t deltaL, deltaR;
int newLdbg, newRdbg;

struct timespec lastCoderRead;

void updateDelta()
{
    newL = (readI2C(fdFPGA(), CODER_LEFT_H) << 8 | readI2C(fdFPGA(), CODER_LEFT_L)) & 0xFFFF;
    newR = (readI2C(fdFPGA(), CODER_RIGHT_H) << 8 | readI2C(fdFPGA(), CODER_RIGHT_L)) & 0xFFFF;
    newLdbg = newL;
    newRdbg = newR;
    deltaL = (abs(oldL - newL) < UINT16_MAX / 2) ? newL - oldL : UINT16_MAX - oldL + newL;
    deltaR = (abs(oldR - newR) < UINT16_MAX / 2) ? newR - oldR : UINT16_MAX - oldR + newR;

    // Verification de valeur abbérante
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    float maxDelta = diffTimeSec(&lastCoderRead, &now) * ABSOLUTE_MAX_VITESSE_ROBOT_CYCP_S;
    if (abs(deltaL) > maxDelta) {
        deltaL = 0;
    }
    if (abs(deltaR) > maxDelta) {
        deltaR = 0;
    }
    lastCoderRead = now;

    oldL = newL;
    oldR = newR;
}

void* TaskPosition(void* pData)
{
    (void)pData;

    resetPosition();
    nbCalcPos = 0;
    lCodTot = 0;
    rCodTot = 0;

    updateDelta();

    for (;;) {

        updateDelta();

        // Calculation
#ifdef INVERSE_L_CODER
        deltaL = -deltaL;
#endif
#ifdef INVERSE_R_CODER
        deltaR = -deltaR;
#endif
        lCodTot += deltaL;
        rCodTot += deltaR;

        float dR = deltaR * AV_PER_CYCLE;
        float dL = deltaL * AV_PER_CYCLE;

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

        usleep(POSITION_INTERVAL * 1000);
    }

    return NULL;
}

void configurePosition()
{
    resetPosition();
    registerDebugVar("lCodTot", ld, &lCodTot);
    registerDebugVar("rCodTot", ld, &rCodTot);
    registerDebugVar("newL", d, &newLdbg);
    registerDebugVar("newR", d, &newRdbg);
    registerDebugVar("xConnu", f, &connu.x);
    registerDebugVar("yConnu", f, &connu.y);
    registerDebugVar("oConnu", f, &connu.o);
    registerDebugVar("nbCalcPos", d, &nbCalcPos);
    pthread_mutex_init(&posConnu, NULL);
    pthread_cond_init(&newPos, NULL);
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
    struct position pos = { 0, 0, 0 };
    setPosition(&pos);
}
