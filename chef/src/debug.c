#include <dirent.h>
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
bool debugConfigured = false;

struct debugArg* listeDebugArgs = NULL;

FILE* debugFd;

int nextId()
{
    DIR* d;
    struct dirent* dir;
    d = opendir("log");

    int maxId = 0, id, ret;

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            ret = sscanf(dir->d_name, "%06d", &id);
            if (ret == 1 && id > maxId) {
                maxId = id;
            }
        }
        closedir(d);
    }
    return maxId + 1;
}

struct timespec debugStart;
struct timespec debugNow;
struct timespec debugEcoule;

int debugInterval = DEBUG_INTERVAL_IDLE;

void debugSetActive(bool active)
{
    debugInterval = active ? DEBUG_INTERVAL_ACTIVE : DEBUG_INTERVAL_IDLE;
}

void* TaskDebug(void* pdata)
{
    (void)pdata;

    clock_gettime(CLOCK_REALTIME, &debugStart);

    fprintf(debugFd, "\n");

    for (;;) {
        clock_gettime(CLOCK_REALTIME, &debugNow);
        if ((debugNow.tv_nsec - debugStart.tv_nsec) < 0) {
            debugEcoule.tv_sec = debugNow.tv_sec - debugStart.tv_sec - 1;
            debugEcoule.tv_nsec = debugNow.tv_nsec - debugStart.tv_nsec + 1000000000UL;
        } else {
            debugEcoule.tv_sec = debugNow.tv_sec - debugStart.tv_sec;
            debugEcoule.tv_nsec = debugNow.tv_nsec - debugStart.tv_nsec;
        }
        fprintf(debugFd, "%ld.%09ld", debugEcoule.tv_sec, debugEcoule.tv_nsec);

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
        fflush(debugFd);

        usleep(debugInterval * 1000);
    }

    return NULL;
}

void configureDebug()
{

    // Génération du nom de fichier
    char path[256];
    sprintf(path, "log/%06d.csv", nextId());

    // Open file
    debugFd = fopen(path, "w");
    if (debugFd == NULL) {
        perror("fopen debug file");
        exit(EXIT_FAILURE);
    }

    printf("Writing log in file: %s\n", path);

    fprintf(debugFd, "time");

    debugConfigured = true;
}

void registerDebugVar(char* name, enum debugArgTypes type, void* var)
{
    if (debugConfigured) {
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
}

void startDebug()
{
    pthread_create(&tDebug, NULL, TaskDebug, NULL);
}

void deconfigureDebug()
{
    if (debugConfigured) {
        pthread_cancel(tDebug);
        fclose(debugFd);
        debugConfigured = false;
        // TODO Vider la liste des arguments
    }
}
