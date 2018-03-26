#include "debug.h"
#include <stdio.h>
#include <unistd.h> // sleep

void* TaskDebug(void* pdata)
{
    (void)pdata;
    struct timespec tim;
    /* tim.tv_sec = 0; */
    /* tim.tv_nsec = 100000000L; */
    tim.tv_sec = 1;
    tim.tv_nsec = 0;

    for (;;) {
        nanosleep(&tim, NULL);
        sendCA(A2CI_DBG, NULL, 0);
    }

    return NULL;
}

void printDebugInfos(struct A2CI_DBGs* debug)
{
    // Position
    printf("← + % .6g; % .6g % .6g°", debug->actuel.x, debug->actuel.y, debug->actuel.o);
    // Frequence de calcul de la position
    printf(" % 5d☼", debug->nbCalcPos);
    // Delta codeuses
    printf(", %5d↿↾%-5d", debug->deltaCoder.dL, debug->deltaCoder.dR);
    // Destination
    printf(", ");
    if (debug->movement == C2AD_BRAKE) {
        printf("⇞ \n");
    } else if (debug->movement == C2AD_FREE) {
        printf("↕ \n");
    } else {
        printf("↑ % .6g; % .6g % .6g°\n", debug->destination.x, debug->destination.y, debug->destination.o);
    }
}

void onA2CI_DBG()
{
    readCA(&debug, sizeof(struct A2CI_DBGs));
    printDebugInfos(&debug);
}

void configureDebug()
{
    registerRxHandler(A2CI_DBG, onA2CI_DBG);
#ifdef REGULARREPORTS
    pthread_create(&tDebug, NULL, TaskDebug, NULL);
#endif
}

void deconfigureDebug()
{
#ifdef REGULARREPORTS
    pthread_cancel(tDebug);
#endif
}
