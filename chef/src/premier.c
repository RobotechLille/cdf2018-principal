#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "CF.h"
#include "debug.h"
#include "ihm.h"
#include "movement.h"
#include "position.h"

int main()
{

    printf("Démarrage...\n");
    if (wiringPiSetup() < 0) {
        fprintf(stderr, "Impossible d'initialiser WiringPi\n");
        exit(EXIT_FAILURE);
    }
    initI2C();
    configureIHM();

    srand(time(NULL));
    configureDebug();
    configureCF();
    configureMovement();
    configurePosition();
    startDebug();

    startIHM();

    deconfigureMovement();
    deconfigurePosition();
    deconfigureCF();
    deconfigureIHM();
    deconfigureDebug();
    return EXIT_SUCCESS;
}
