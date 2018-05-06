#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "motor.h"
#include "movement.h"

pthread_t tMovement;
struct position cons;
pthread_mutex_t movCons;
pthread_mutex_t movEnableMutex;
pthread_cond_t movEnableCond;
bool movEnableBool;

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
unsigned int nbCalcCons;

void configureMovement()
{
    stop();

    nbCalcCons = 0;
    pthread_mutex_init(&movCons, NULL);

    pthread_mutex_init(&movEnableMutex, NULL);
    pthread_cond_init(&movEnableCond, NULL);
    movEnableBool = false;

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

    disableConsigne();
}

void setDestination(struct position* pos)
{
    pthread_mutex_lock(&movCons);
    memcpy(&cons, pos, sizeof(struct position));
    pthread_mutex_unlock(&movCons);
}

float angleGap(float target, float actual)
{
    return fmod(target - actual + M_PI, 2 * M_PI) - M_PI;
}

void* TaskMovement(void* pData)
{
    (void)pData;

    unsigned int lastPosCalc = 0;
    struct position connu;

    oRetenu = true;
    dRetenu = true;

    for (;;) {

        // Test if enabled
        pthread_mutex_lock(&movEnableMutex);
        while (!movEnableBool) {
            pthread_cond_wait(&movEnableCond, &movEnableMutex);
        }
        pthread_mutex_unlock(&movEnableMutex);

        lastPosCalc = getPositionNewer(&connu, lastPosCalc);
        // Destination → ordre
        pthread_mutex_lock(&movCons);
        xDiff = cons.x - connu.x;
        yDiff = cons.y - connu.y;
        oEcart = angleGap(cons.o, connu.o);

        dDirEcart = hypotf(xDiff, yDiff);
        oDirEcart = angleGap(atan2(yDiff, xDiff), connu.o);
        pthread_mutex_unlock(&movCons);

        if (dDirEcart > D_DIR_ECART_MAX) {
            oRetenu = true;
        } else if (dDirEcart < D_DIR_ECART_MIN) {
            oRetenu = false;
        }
        oErr = oRetenu ? oDirEcart : oEcart;

        float oDirEcartAbs = fabs(oDirEcart);
        if (oDirEcartAbs > O_DIR_ECART_MAX) {
            dRetenu = true;
        } else if (oDirEcartAbs < O_DIR_ECART_MIN) {
            dRetenu = false;
        }
        dErr = dRetenu ? 0 : dDirEcart;

        // Ordre → Volt
        float dErrRev = dErr / WHEEL_PERIMETER;
        float oErrRev = O_GAIN * oErr * DISTANCE_BETWEEN_WHEELS / WHEEL_PERIMETER;

        lErr = dErrRev - oErrRev;
        rErr = dErrRev + oErrRev;

        // PID
        float lVoltCons = P * lErr;
        float rVoltCons = P * rErr;

        setMoteurTension(lVoltCons, rVoltCons);

        nbCalcCons++;
    }

    return NULL;
}

void deconfigureMovement()
{
    stop();
    pthread_cancel(tMovement);
}

void enableConsigne()
{
    pthread_mutex_lock(&movEnableMutex);
    movEnableBool = true;
    pthread_cond_signal(&movEnableCond);
    pthread_mutex_unlock(&movEnableMutex);
}

void disableConsigne()
{
    pthread_mutex_lock(&movEnableMutex);
    movEnableBool = false;
    // No signal here, will be disabled on next run
    pthread_mutex_unlock(&movEnableMutex);
}
