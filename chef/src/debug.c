#include "debug.h"
#include <stdio.h>
#include <unistd.h> // sleep
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "position.h"


void* TaskDebug(void* pdata)
{
    (void)pdata;
    struct timespec tim; // 100 ms
    tim.tv_sec = 0;
    tim.tv_nsec = 100000000L;
    /* tim.tv_sec = 1; */
    /* tim.tv_nsec = 0; */

    char line[1024];
    clock_t t;

    for (;;) {

        // Calculating time index
        t = clock() - debugStart;

        // Generating line
        sprintf(line, "%ld,%d,%ld,%ld\n", t, nbCalcPos, lCodTot, rCodTot);

        // Writing
        write(debugFd, line, strlen(line));

        // Sleeping
        nanosleep(&tim, NULL);
    }

    return NULL;
}

void configureDebug()
{
    debugStart = clock();

    // Génération du nom de fichier
    char path[256];
    time_t startTime;
    time(&startTime);
    sprintf(path, "log/%ld.csv", startTime);

    // Open file
    debugFd = open(path, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (debugFd < 0) {
        fprintf(stderr, "Impossible d'ouvrir le fichier '%s', debug désactivé.\n", path);
        return;
    }

    char header[] = "time,nbCalcPos,lCodTot,rCodTot\n";
    write(debugFd, header, strlen(header));

    pthread_create(&tDebug, NULL, TaskDebug, NULL);
}

void deconfigureDebug()
{
    pthread_cancel(tDebug);
    close(debugFd);
}
