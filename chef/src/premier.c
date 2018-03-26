#include <stdlib.h>
#include <stdio.h>
#include <time.h> // random seed
#include <pthread.h>
#include <unistd.h> // sleep

#include "CA.h"
#include "movement.h"
#include "debug.h"

#define TEMPSMAX 60

void* TaskParcours(void *pdata)
{
    (void) pdata;

    struct position pos;
    for (;;) {
        pos.x = (int) (rand()*200.0/RAND_MAX);
        pos.y = (int) (rand()*100.0/RAND_MAX);
        pos.o = (int) (rand()*360.0/RAND_MAX);
        aller(&pos);
        sleep(1);
        brake();
        sleep(2);
    }

    printf("Fin du parcours\n");
    return NULL;
}

int main()
{

    printf("Démarrage...\n");
    configureCA();
    configureDebug();
    srand(time(NULL));

    /* printf("Synchronisation avec le Raspberry Pi\n"); // TODO */

    /* printf("En attente de la tirette...\n"); // TODO */
    printf("C'est parti !\n");

    /* pthread_t tParcours; */
    /* pthread_create(&tParcours, NULL, TaskParcours, NULL); */

    sleep(TEMPSMAX);

    printf("Fin des %d secondes\n", TEMPSMAX);
    /* pthread_cancel(tParcours); */


    stop();

    deconfigureDebug();
    deconfigureCA();
    return EXIT_SUCCESS;
}
