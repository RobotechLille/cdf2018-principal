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
    printf("← Position actuelle (%f; %f) (%f°)", debug->actuel.x, debug->actuel.y, debug->actuel.o);
    printf(", destination : ");
    if (debug->movement == C2AD_BRAKE) {
        printf("ne pas bouger\n");
    } else if (debug->movement == C2AD_FREE) {
        printf("là où le vent l'emporte\n");
    } else {
        printf("(%f; %f) (%f°)\n", debug->destination.x, debug->destination.y, debug->destination.o);
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
