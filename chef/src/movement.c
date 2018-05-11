#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
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
float oEcart;
float dDirEcart;
float oDirEcart;
float dErr;
float oErr;
bool oRetenu;
bool dRetenu;
float lErr;
float rErr;
float lErrPrev;
float rErrPrev;
unsigned int nbCalcCons;

void configureMovement()
{
    stop();

    configureMotor();
#ifdef ENABLE_SECURITE
    configureSecurite();
#endif

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
    registerDebugVar("oEcart", f, &oEcart);
    registerDebugVar("dErr", f, &dErr);
    registerDebugVar("oErr", f, &oErr);
    registerDebugVar("dDirEcart", f, &dDirEcart);
    registerDebugVar("oDirEcart", f, &oDirEcart);
    registerDebugVar("dRetenu", d, &dRetenu);
    registerDebugVar("oRetenu", d, &oRetenu);
    registerDebugVar("lErr", f, &lErr);
    registerDebugVar("rErr", f, &rErr);
    registerDebugVar("nbCalcCons", d, &nbCalcCons);
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

float angleGap(float target, float actual)
{
    float ret = fmodf(target - actual + M_PI, 2 * M_PI) - M_PI;
    return ret;
}

float angleGap180(float target, float actual, float* dist)
{
    if (fabs(fmodf(target - actual + M_PI, 2 * M_PI) - M_PI) > M_PI_2) {
        *dist = -*dist;
    }
    return fmodf(target - actual + M_PI_2, M_PI) - M_PI_2;
}

void* TaskMovement(void* pData)
{
    (void)pData;

    unsigned int lastPosCalc = 0;
    struct position connu;

    for (;;) {
        // Attend instruction
        printf("Wait instruction\n");
        pthread_mutex_lock(&movInstructionMutex);
        while (!movInstructionBool) {
            pthread_cond_wait(&movInstructionCond, &movInstructionMutex);
        } // Début de l'instruction

        printf("Oriente dir\n");
        // Oriente vers direction
        // TODO Marche arrière
        do {

            lastPosCalc = getPositionNewer(&connu, lastPosCalc);

            xDiff = cons.x - connu.x;
            yDiff = cons.y - connu.y;
            oDirEcart = angleGap(atan2(yDiff, xDiff), connu.o);
            float oErrRev = oDirEcart * DISTANCE_BETWEEN_WHEELS / WHEEL_PERIMETER;
            float lVolt = -oErrRev * P + (oErrRev > 0 ? -M : M);
            float rVolt = oErrRev * P + (oErrRev > 0 ? M : -M);
            setMoteurTension(lVolt, rVolt);

            nbCalcCons++;
        } while (fabsf(oDirEcart) > O_DIR_ECART_MIN);
        brake();
        usleep(500 * 1000);

        printf("Avance dir\n");
        // Avance vers direction
        do {

            lastPosCalc = getPositionNewer(&connu, lastPosCalc);

            xDiff = cons.x - connu.x;
            yDiff = cons.y - connu.y;
            dDirEcart = hypotf(xDiff, yDiff);
            oDirEcart = angleGap(atan2(yDiff, xDiff), connu.o);

#ifdef ENABLE_SECURITE
            float distDevant, distDerriere;
            getDistance(&distDevant, &distDerriere);
            float maxEcart = fmax(0, distDevant - SECURITE_MARGE);
            float minEcart = fmin(0, -distDerriere + SECURITE_MARGE);
            dErr = fmin(maxEcart, fmax(minEcart, dDirEcart));
#else
            dErr = dDirEcart;
#endif

            float dErrRev = dErr / WHEEL_PERIMETER;
            float oErrRev = O_GAIN * oDirEcart * DISTANCE_BETWEEN_WHEELS / WHEEL_PERIMETER;

            lErr = dErrRev - oErrRev;
            rErr = dErrRev + oErrRev;
            float lVolt = lErr * P + (lErr > 0 ? M : -M);
            float rVolt = rErr * P + (rErr > 0 ? M : -M);
            setMoteurTension(lVolt, rVolt);

            nbCalcCons++;
        } while (fabsf(dDirEcart) > D_DIR_ECART_MIN);
        brake();
        usleep(500 * 1000);

        printf("Orientation finale\n");
        // Orientation finale (si nécessaire)
        if (!isnan(cons.o)) {
            do {

                lastPosCalc = getPositionNewer(&connu, lastPosCalc);
                oDirEcart = angleGap(cons.o, connu.o);
                float oErrRev = oDirEcart * DISTANCE_BETWEEN_WHEELS / WHEEL_PERIMETER;
                float lVolt = -oErrRev * P + (oErrRev > 0 ? -M : M);
                float rVolt = oErrRev * P + (oErrRev > 0 ? M : -M);
                setMoteurTension(lVolt, rVolt);

                nbCalcCons++;
            } while (fabsf(oDirEcart) > O_DIR_ECART_MIN);
            brake();
            usleep(500 * 1000);
        }

        movInstructionBool = false; // Fin de l'instruction
        pthread_cond_signal(&movInstructionCond);
        pthread_mutex_unlock(&movInstructionMutex);
    }

    return NULL;
}

void deconfigureMovement()
{
    stop();
    pthread_cancel(tMovement);
    deconfigureSecurite();
    deconfigureMotor();
}
