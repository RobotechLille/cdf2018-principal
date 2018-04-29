#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // sleep

#include "debug.h"

// Variables globales
pthread_t tDebug;

struct debugArg* listeDebugArgs = NULL;

FILE* debugFd;

void* TaskDebug(void* pdata)
{
    (void)pdata;

    clock_t debugStart;
    debugStart = clock();

    struct timespec tim; // 100 ms
    tim.tv_sec = 0;
    tim.tv_nsec = 100000000L;

    fprintf(debugFd, "\n");

    for (;;) {
        clock_t t = clock() - debugStart;
        fprintf(debugFd, "%ld", t);

        struct debugArg* arg = listeDebugArgs;
        while (arg != NULL) {
            switch (arg->type) {
            case d:
                fprintf(debugFd, ",%d", *((int*)arg->var));
                break;
            case ld:
                fprintf(debugFd, ",%ld", *((long int*)arg->var));
                break;
            case f:
                fprintf(debugFd, ",%f", *((float*)arg->var));
                break;
            case lf:
                fprintf(debugFd, ",%f", *((double*)arg->var));
                break;
            case s:
                fprintf(debugFd, ",%s", *((char**)arg->var));
                break;
            default:
                fprintf(debugFd, ",?");
                break;
            }

            arg = arg->next;
        }
        fprintf(debugFd, "\n");

        nanosleep(&tim, NULL);
    }

    return NULL;
}

void configureDebug()
{

    // Génération du nom de fichier
    char path[256];
    time_t startTime;
    time(&startTime);
    sprintf(path, "log/%ld.csv", startTime);

    // Open file
    debugFd = fopen(path, "w");
    if (debugFd == NULL) {
        perror("fopen debug file");
        return;
    }

    fprintf(debugFd, "time");
}

void registerDebugVar(char* name, enum debugArgTypes type, void* var)
{
    fprintf(debugFd, ",%s", name);

    struct debugArg* arg = NULL;
    struct debugArg** addrArg = &listeDebugArgs;
    while (*addrArg != NULL) {
        addrArg = &((*addrArg)->next);
    }

    arg = malloc(sizeof(struct debugArg));
    arg->type = type;
    arg->var = var;
    arg->next = NULL;

    *addrArg = arg;
}

void startDebug()
{
    pthread_create(&tDebug, NULL, TaskDebug, NULL);
}

void deconfigureDebug()
{
    pthread_cancel(tDebug);
    fclose(debugFd);
    // TODO Vider la liste des arguments
}
