#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep

#include "CF.h"
#include "position.h"

#define TEMPSMAX 10

int main()
{

    printf("Démarrage...\n");
    configureCF();
    configurePosition();
    srand(time(NULL));

    printf("C'est parti !\n");

    sleep(TEMPSMAX);

    printf("Fin des %d secondes\n", TEMPSMAX);


    deconfigureCF();
    return EXIT_SUCCESS;
}

