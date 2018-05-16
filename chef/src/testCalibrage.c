#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "actionneurs.h"
#include "calibrage.h"
#include "debug.h"
#include "i2c.h"
#include "motor.h"
#include "movement.h"
#include "position.h"

pthread_mutex_t sRunning;

void endRunning(int signal)
{
    (void)signal;
    pthread_mutex_unlock(&sRunning);
}

int main()
{

    if (wiringPiSetup() < 0) {
        fprintf(stderr, "Impossible d'initialiser WiringPi\n");
        exit(EXIT_FAILURE);
    }
    initI2C();
    srand(time(NULL));

    configureDebug();
    configurePosition();
    configureMovement();
    debugSetActive(true);
    startDebug();

    calibrer(false);

    deconfigureMovement();
    deconfigurePosition();
    deconfigureDebug();
    return EXIT_SUCCESS;
}
