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
enum movStates etat;
unsigned int nbCalcCons;

bool secuAv = true;
bool secuAr = true;

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
    registerDebugVar("etat", d, &etat);
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

    bool reverse;
    bool obstacle;
    etat = quelconque;

    for (;;) {

        lastPosCalc = getPositionNewer(&connu, lastPosCalc);

        xDiff = cons.x - connu.x;
        yDiff = cons.y - connu.y;
        dDirEcart = hypotf(xDiff, yDiff);
        oDirEcart = angleMod(atan2(yDiff, xDiff) - connu.o);
        oConsEcart = isnan(cons.o) ? 0 : angleMod(cons.o - connu.o);

        if ((reverse = fabsf(oDirEcart) > M_PI_2)) {
            dDirEcart = -dDirEcart;
            oDirEcart = angleMod(atan2(yDiff, xDiff) - connu.o + M_PI);
        }

        // Selection de l'état suivant
        switch (etat) {
        case quelconque:
            if (fabs(oDirEcart) < O_DIR_ECART_MIN) {
                etat = direction;
            }
            break;
        case direction:
            if (fabs(getAnglVitesse()) < O_VIT_MIN && oVolt < O_TENSION_MIN) {
                etat = approche;
            }
            break;
        case approche:
            if (fabs(dDirEcart) < D_DIR_ECART_MIN) {
                etat = arret;
            }
            break;
        case arret:
            if (fabs(dDirEcart) > D_DIR_ECART_MAX) {
                etat = quelconque;
            } else if (fabs(getAbsVitesse()) < D_VIT_MIN && dVolt < D_TENSION_MIN) {
                etat = orientation;
            }
            break;
        case orientation:
            if (fabs(dDirEcart) > D_DIR_ECART_MAX) {
                etat = quelconque;
            } else if (fabs(oConsEcart) < O_ECART_MIN) {
                etat = oriente;
            }
            break;
        case oriente:
            if (fabs(dDirEcart) > D_DIR_ECART_MAX) {
                etat = quelconque;
            } else if (fabs(oConsEcart) > O_ECART_MAX) {
                etat = orientation;
            } else if (fabs(getAnglVitesse()) < O_VIT_MIN && oVolt < O_TENSION_MIN) {
                etat = fini;
            }
            break;
        case fini:
            if (fabs(dDirEcart) > D_DIR_ECART_MAX) {
                etat = quelconque;
            } else if (fabs(oConsEcart) > O_ECART_MAX) {
                etat = orientation;
            }
            break;
        }

        // Application des directives d'état
        switch (etat) {
        case quelconque:
        case direction:
            oEcart = oDirEcart;
            dEcart = 0;
            break;
        case approche:
            oEcart = oDirEcart;
            dEcart = dDirEcart;
            break;
        case arret:
            oEcart = 0;
            dEcart = dDirEcart;
            break;
        case orientation:
        case oriente:
            oEcart = oConsEcart;
            dEcart = 0;
            break;
        case fini:
            oEcart = 0;
            dEcart = 0;
            break;
        }

#ifdef ENABLE_SECURITE
        float av, ar;
        getDistance(&av, &ar);
        if (!reverse) {
            obstacle = secuAv && av < MARGE_SECURITE;
            /* dEcart = fmax(av, dEcart); */
        } else {
            obstacle = secuAr && ar < MARGE_SECURITE;
            /* dEcart = fmin(-ar, dEcart); */
        }
#endif

        // Carotte
        dErr = fcap(dEcart, CAROTTE_DISTANCE);
        oErr = fcap(oEcart * DISTANCE_BETWEEN_WHEELS, CAROTTE_ANGLE);

        dVolt = updatePID(&dPid, dErr);
        oVolt = updatePID(&oPid, oErr);

        float lVolt = dVolt - oVolt;
        float rVolt = dVolt + oVolt;

        pthread_mutex_lock(&movInstructionMutex);
        if (movInstructionBool) {
            if (obstacle || etat == fini) {
                brake();
            } else {
                setMoteurTension(lVolt, rVolt);
            }
            pthread_cond_signal(&movInstructionCond);
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
    etat = quelconque;
    pthread_mutex_unlock(&movInstructionMutex);
}

void setDestination(struct position* pos)
{
    pthread_mutex_lock(&movInstructionMutex);
    etat = quelconque;
    memcpy(&cons, pos, sizeof(struct position));
    movInstructionBool = true;
    pthread_cond_signal(&movInstructionCond);
    pthread_mutex_unlock(&movInstructionMutex);
}

void setSecurite(bool av, bool ar)
{
    pthread_mutex_lock(&movInstructionMutex);
    secuAv = av;
    secuAr = ar;
    pthread_mutex_unlock(&movInstructionMutex);
}

void waitDestination()
{
    pthread_mutex_lock(&movInstructionMutex);
    while (etat != fini) {
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
