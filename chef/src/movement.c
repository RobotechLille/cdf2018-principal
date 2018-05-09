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
pthread_mutex_t movCons;
pthread_mutex_t movEnableMutex;
pthread_cond_t movEnableCond;
bool movEnableBool;
pthread_mutex_t movDoneMutex;
pthread_cond_t movDoneCond;
bool movDoneBool;

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
float lVolt;
float rVolt;
unsigned int nbCalcCons;

struct timespec pidStart;
struct timespec pidNow;
struct timespec pidEcoule;

void configureMovement()
{
    stop();

    configureSecurite();

    nbCalcCons = 0;
    pthread_mutex_init(&movCons, NULL);

    pthread_mutex_init(&movEnableMutex, NULL);
    pthread_cond_init(&movEnableCond, NULL);
    movEnableBool = false;
    pthread_mutex_init(&movDoneMutex, NULL);
    pthread_cond_init(&movDoneCond, NULL);
    movDoneBool = false;

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
    registerDebugVar("lVolt", f, &lVolt);
    registerDebugVar("rVolt", f, &rVolt);
    registerDebugVar("nbCalcCons", d, &nbCalcCons);

    disableConsigne();
}

void setDestination(struct position* pos)
{
    pthread_mutex_lock(&movCons);
    memcpy(&cons, pos, sizeof(struct position));
    pthread_mutex_unlock(&movCons);
}

void waitDestination()
{
    pthread_mutex_lock(&movDoneMutex);
    while (!movDoneBool) {
        pthread_cond_wait(&movDoneCond, &movDoneMutex);
    }
    pthread_mutex_unlock(&movDoneMutex);
}

float angleGap(float target, float actual)
{
    return fmod(target - actual + M_PI, 2 * M_PI) - M_PI;
}

float angleGap180(float target, float actual, float* dist)
{
    if (fabs(fmod(target - actual + M_PI, 2 * M_PI) - M_PI) > M_PI_2) {
        *dist = -*dist;
    }
    return fmod(target - actual + M_PI_2, M_PI) - M_PI_2;
}

void* TaskMovement(void* pData)
{
    (void)pData;

    unsigned int lastPosCalc = 0;
    struct position connu;

    oRetenu = true;
    dRetenu = true;

    float lErrInteg = 0;
    float rErrInteg = 0;

    clock_gettime(CLOCK_REALTIME, &pidStart);

    for (;;) {

        // Test if enabled
        pthread_mutex_lock(&movEnableMutex);
        while (!movEnableBool) {
            pthread_cond_wait(&movEnableCond, &movEnableMutex);
        }
        pthread_mutex_unlock(&movEnableMutex);

        // Wait for new calculation if not calculated yet
        lastPosCalc = getPositionNewer(&connu, lastPosCalc);

        // Destination → ordre
        bool angleInsignifiant = isnan(cons.o);
        pthread_mutex_lock(&movCons);
        xDiff = cons.x - connu.x;
        yDiff = cons.y - connu.y;
        oEcart = angleInsignifiant ? 0 : angleGap(cons.o, connu.o);

        // Distance d'écart entre la position actuelle et celle de consigne
        dDirEcart = hypotf(xDiff, yDiff);
        // Écart entre l'angle actuel et celui orienté vers la position de consigne
        // Si l'angle se trouve à gauche du cercle trigo, on le remet à droite
        // et on inverse la direction
        oDirEcart = angleGap180(atan2(yDiff, xDiff), connu.o, &dDirEcart);
        pthread_mutex_unlock(&movCons);

        // Si on est loin de la consigne, l'angle cible devient celui orienté vers la consigne
        if (dDirEcart > D_DIR_ECART_MAX) {
            oRetenu = true;
            // Si on est proche de la consigne, l'angle cible devient celui voulu par la consigne
        } else if (dDirEcart < D_DIR_ECART_MIN) {
            oRetenu = false;
        }
        oErr = oRetenu ? oDirEcart : oEcart;

        float oDirEcartAbs = fabs(oDirEcart);
        // Si l'écart avec l'angle orienté vers la consigne est grand, la distance cible devient 0
        // pour se réorienter vers l'angle de la consigne
        if (oDirEcartAbs > O_DIR_ECART_MAX) {
            dRetenu = true;
            // Si l'écart avec l'angle orienté vers la consigne est petit, la distance cible devient
            // la distance entre la position actuelle et la consigne
        } else if (oDirEcartAbs < O_DIR_ECART_MIN) {
            dRetenu = false;
        }
        dErr = dRetenu ? 0 : dDirEcart;

        // Limitation par les valeurs des capteurs
        float avant, arriere;
        getDistance(&avant, &arriere);
        dErr = fmaxf(-arriere, fminf(avant, dErr));

        // Calcul si on est arrivé
        pthread_mutex_lock(&movDoneMutex);
        clock_gettime(CLOCK_REALTIME, &pidNow);
        movDoneBool = !dRetenu && !oRetenu && dDirEcart < D_CONS_THRESOLD && oEcart < O_CONS_THRESOLD;
        if (movDoneBool) {
            pthread_cond_signal(&movDoneCond);
        }
        pthread_mutex_unlock(&movDoneMutex);

        // Ordre → Volt
        // Nombre de rotation nécessaire sur les deux roues dans le même sens pour arriver à la distance voulue
        float dErrRev = dErr / WHEEL_PERIMETER;
        // Nombre de rotation nécessaire sur les deux roues dans le sens inverse pour arriver à l'angle voulu
        float oErrRev = oErr * DISTANCE_BETWEEN_WHEELS / WHEEL_PERIMETER;

        // Si on est en avancement on applique une grande priorité au retour sur la ligne
        if (!dRetenu && oRetenu) {
            oErrRev *= O_GAIN;
        }

        lErr = dErrRev - oErrRev;
        rErr = dErrRev + oErrRev;

        // PID
        // Calcul du temps écoulé par rapport à la dernière mesure
        clock_gettime(CLOCK_REALTIME, &pidNow);
        if ((pidNow.tv_nsec - pidStart.tv_nsec) < 0) {
            pidEcoule.tv_sec = pidNow.tv_sec - pidStart.tv_sec - 1;
            pidEcoule.tv_nsec = pidNow.tv_nsec - pidStart.tv_nsec + 1000000000UL;
        } else {
            pidEcoule.tv_sec = pidNow.tv_sec - pidStart.tv_sec;
            pidEcoule.tv_nsec = pidNow.tv_nsec - pidStart.tv_nsec;
        }
        // Enregistrement de cette mesure comme la dernière mesure
        pidStart.tv_sec = pidNow.tv_sec;
        pidStart.tv_nsec = pidNow.tv_nsec;
        float timeStep = pidEcoule.tv_sec + pidStart.tv_nsec * 1E-9;

        // Calcul des facteurs dérivé et intégrale
        lErrInteg += (lErr + lErrPrev) / 2 * timeStep;
        float lErrDeriv = (lErr - lErrPrev) / timeStep;
        lErrPrev = lErr;

        rErrInteg += (rErr + rErrPrev) / 2 * timeStep;
        float rErrDeriv = (rErr - rErrPrev) / timeStep;
        rErrPrev = rErr;

        // Calcul de la commande
        lVolt = P * lErr + I * lErrInteg + D * lErrDeriv;
        rVolt = P * rErr + I * rErrInteg + D * rErrDeriv;

        // Envoi de la commande
        setMoteurTension(lVolt, rVolt);

        nbCalcCons++;
    }

    return NULL;
}

void deconfigureMovement()
{
    stop();
    pthread_cancel(tMovement);
    deconfigureSecurite();
}

void enableConsigne()
{
    pthread_mutex_lock(&movEnableMutex);
    clock_gettime(CLOCK_REALTIME, &pidNow);
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
