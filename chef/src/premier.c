#include <stdlib.h>
#include <stdio.h>
#include <time.h> // random seed
#include <pthread.h>
#include <unistd.h> // sleep

#include "CF.h"
#include "movement.h"
#include "position.h"
#include "debug.h"

#define TEMPSMAX 60

void* TaskParcours(void *pdata)
{
    (void) pdata;

    /* struct position pos; */
    /* for (;;) { */
    /*     pos.x = (int) (rand()*200.0/RAND_MAX); */
    /*     pos.y = (int) (rand()*100.0/RAND_MAX); */
    /*     pos.o = (int) (rand()*360.0/RAND_MAX); */
    /*     aller(&pos); */
    /*     sleep(1); */
    /*     brake(); */
    /*     sleep(2); */
    /* } */

    struct timespec tim; // 10 ms
    tim.tv_sec = 0;
    tim.tv_nsec = 10000000L;

#define RAMP_TIME 100
#define MAX_VIT MOT_MAX_V

    for (;;) {
        // ↗
        for (int i = 0; i < RAMP_TIME; i++) {
            float p = (float) i / (float) RAMP_TIME;
            changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V);
            nanosleep(&tim, NULL);
        }
        changerMoteurs(MOT_MAX_V, MOT_MAX_V);
        // ↑
        sleep(2);
        // ↘
        for (int i = 0; i < RAMP_TIME; i++) {
            float p = (float) i / (float) RAMP_TIME;
            p = 1 - p;
            changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V);
            nanosleep(&tim, NULL);
        }
        sleep(5);
    }

    printf("Fin du parcours\n");
    return NULL;
}

int main()
{

    printf("Démarrage...\n");
    configureCF();
    configureDebug();
    configureMovement();
    configurePosition();
    srand(time(NULL));

    /* printf("En attente de la tirette...\n"); // TODO */
    printf("C'est parti !\n");

    pthread_t tParcours;
    pthread_create(&tParcours, NULL, TaskParcours, NULL);

    sleep(TEMPSMAX);

    printf("Fin des %d secondes\n", TEMPSMAX);
    /* pthread_cancel(tParcours); */


    /* stop(); */

    deconfigureMovement();
    deconfigurePosition();
    deconfigureDebug();
    deconfigureCF();
    return EXIT_SUCCESS;
}
