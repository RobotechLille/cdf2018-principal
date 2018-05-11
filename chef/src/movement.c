#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "debug.h"
#include "dimensions.h"
#include "motor.h"
#include "movement.h"
#include "securite.h"

pthread_t tMovement;
struct position cons;

// Si une nouvelle instruction est disponible
pthread_mutex_t movInstructionMutex;
pthread_cond_t movInstructionCond;
bool movInstructionBool;

float xDiff;
float yDiff;
float dEcart;
float oEcart;
float oDirEcart;
float dDirEcart;
float oConsEcart;
float dErr;
float oErr;
float dVolt;
float oVolt;
float lErr;
float rErr;
unsigned int nbCalcCons;

void configureMovement()
{
    stop();

    configureMotor();
    configureSecurite();

    nbCalcCons = 0;

    pthread_mutex_init(&movInstructionMutex, NULL);
    pthread_cond_init(&movInstructionCond, NULL);
    movInstructionBool = false;

    pthread_create(&tMovement, NULL, TaskMovement, NULL);

    registerDebugVar("xCons", f, &cons.x);
    registerDebugVar("yCons", f, &cons.y);
    registerDebugVar("oCons", f, &cons.o);
    registerDebugVar("xDiff", f, &xDiff);
    registerDebugVar("yDiff", f, &yDiff);
    registerDebugVar("dErr", f, &dErr);
    registerDebugVar("oErr", f, &oErr);
    registerDebugVar("dVolt", f, &dVolt);
    registerDebugVar("oVolt", f, &oVolt);
    registerDebugVar("dDirEcart", f, &dDirEcart);
    registerDebugVar("oDirEcart", f, &oDirEcart);
    registerDebugVar("dEcart", f, &dEcart);
    registerDebugVar("oEcart", f, &oEcart);
    registerDebugVar("oConsEcart", f, &oConsEcart);
    registerDebugVar("lErr", f, &lErr);
    registerDebugVar("rErr", f, &rErr);
    registerDebugVar("nbCalcCons", d, &nbCalcCons);
}

float angleMod(float angle)
{
    return fmodf(angle + M_PI, 2 * M_PI) - M_PI;
}

float fcap(float value, float cap)
{
    if (value > 0) {
        return fmin(value, cap);
    } else {
        return fmax(value, -cap);
    }
}

void* TaskMovement(void* pData)
{
    (void)pData;

    unsigned int lastPosCalc = 0;
    struct position connu;

    struct PID dPid;
    struct PID oPid;

    initPID(&dPid, D_KP, D_KI, D_KD);
    initPID(&oPid, O_KP, O_KI, O_KD);

    bool orienteDestination = false;
    bool procheDestination = false;
    bool orienteConsigne = false;
    bool reverse;

    for (;;) {

        lastPosCalc = getPositionNewer(&connu, lastPosCalc);

        xDiff = cons.x - connu.x;
        yDiff = cons.y - connu.y;
        dDirEcart = hypotf(xDiff, yDiff);
        oDirEcart = angleMod(atan2(yDiff, xDiff) - connu.o);
        oConsEcart = angleMod(cons.o - connu.o);

        if ((reverse = fabsf(oDirEcart) > M_PI_2)) {
            dDirEcart = -dDirEcart;
            oDirEcart = angleMod(oDirEcart + M_PI);
        }

        if (fabsf(oDirEcart) < O_DIR_ECART_MIN) {
            orienteDestination = true;
        } else if (fabsf(oDirEcart) > O_DIR_ECART_MAX) {
            orienteDestination = false;
        }

        if (fabsf(dDirEcart) < D_DIR_ECART_MIN) {
            procheDestination = true;
        } else if (fabsf(dDirEcart) > D_DIR_ECART_MAX) {
            procheDestination = false;
        }

        if (fabsf(oConsEcart) < O_ECART_MIN) {
            orienteConsigne = true;
        } else if (fabsf(oConsEcart) > O_ECART_MAX) {
            orienteConsigne = false;
        }

        // Carotte
        dEcart = (orienteDestination && !procheDestination) ? dDirEcart : 0;
        dErr = fcap(dEcart, CAROTTE_DISTANCE);
        oEcart = procheDestination ? oConsEcart : oDirEcart;
        oErr = fcap(oEcart * DISTANCE_BETWEEN_WHEELS, CAROTTE_ANGLE);

        dVolt = updatePID(&dPid, dErr);
        oVolt = updatePID(&oPid, oErr);

        float lVolt = dVolt - oVolt;
        float rVolt = dVolt + oVolt;

        pthread_mutex_lock(&movInstructionMutex);
        if (movInstructionBool) {
            if (procheDestination && orienteConsigne) {
                brake();
            } else {
                setMoteurTension(lVolt, rVolt);
            }
        }
        pthread_mutex_unlock(&movInstructionMutex);

        nbCalcCons++;
    }

    return NULL;
}

void enableAsservissement()
{
    pthread_mutex_lock(&movInstructionMutex);
    movInstructionBool = true;
    pthread_mutex_unlock(&movInstructionMutex);
}

void disableAsservissement()
{
    pthread_mutex_lock(&movInstructionMutex);
    movInstructionBool = false;
    pthread_mutex_unlock(&movInstructionMutex);
}

void setDestination(struct position* pos)
{
    pthread_mutex_lock(&movInstructionMutex);
    memcpy(&cons, pos, sizeof(struct position));
    movInstructionBool = true;
    pthread_cond_signal(&movInstructionCond);
    pthread_mutex_unlock(&movInstructionMutex);
}

void waitDestination()
{
    pthread_mutex_lock(&movInstructionMutex);
    while (movInstructionBool) {
        pthread_cond_wait(&movInstructionCond, &movInstructionMutex);
    }
    pthread_mutex_unlock(&movInstructionMutex);
}

void deconfigureMovement()
{
    stop();
    pthread_cancel(tMovement);
    deconfigureSecurite();
    deconfigureMotor();
}
