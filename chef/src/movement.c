#include "movement.h"
#include "CA.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void onC2AD_STOP()
{
    // On considère que l'arrêt se fait très rapidement pour ne pas
    // avoir à attendre le signal de retour de C2AD_STOP
    registerRxHandler(C2AD_STOP, NULL);
}

void stop()
{
    printf("→ Arrêt\n");
    registerRxHandler(C2AD_STOP, onC2AD_STOP);
    sendCA(C2AD_STOP, NULL, 0);
}

void onC2AD_BRAKE()
{
    // On considère que l'arrêt se fait très rapidement pour ne pas
    // avoir à attendre le signal de retour de C2AD_BRAKE
    registerRxHandler(C2AD_BRAKE, NULL);
}

void brake()
{
    printf("→ Frein\n");
    registerRxHandler(C2AD_BRAKE, onC2AD_BRAKE);
    sendCA(C2AD_BRAKE, NULL, 0);
}

// Inspiré de https://stackoverflow.com/a/1760819
pthread_mutex_t reponseMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reponseCond = PTHREAD_COND_INITIALIZER;
bool attenteReponse = false;

void onC2AD_GOTO()
{
    pthread_mutex_lock(&reponseMutex);
    attenteReponse = false;
    pthread_cond_signal(&reponseCond);
    pthread_mutex_unlock(&reponseMutex);
}

void aller(struct position* pos)
{
    printf("→ Déplacement vers (%f; %f) (%f°)\n", pos->x, pos->y, pos->o);

    if (attenteReponse) {
        printf("Déjà en déplacement !\n");
        exit(EXIT_FAILURE);
    }
    attenteReponse = true;

    registerRxHandler(C2AD_GOTO, onC2AD_GOTO);
    sendCA(C2AD_GOTO, (struct C2AD_GOTOs*)pos, sizeof(struct C2AD_GOTOs));

    pthread_mutex_lock(&reponseMutex);
    while (attenteReponse) {
        pthread_cond_wait(&reponseCond, &reponseMutex);
    }
    pthread_mutex_unlock(&reponseMutex);

    registerRxHandler(C2AD_GOTO, NULL);

    printf("← Arrivé à destination\n");
}
