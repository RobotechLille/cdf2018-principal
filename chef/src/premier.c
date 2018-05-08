#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "CF.h"
#include "actionneurs.h"
#include "debug.h"
#include "i2c.h"
#include "ihm.h"
#include "imu.h"
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
    configureIHM();
    configureCF();
    configureIMU();
    configureActionneurs();
    configurePosition();
    configureMovement();
    startDebug();
    startIHM();

    // Bloque jusqu'à l'arrivée d'un signal
    pthread_mutex_init(&sRunning, NULL);
    signal(SIGINT, endRunning);
    signal(SIGTERM, endRunning);
    signal(SIGQUIT, endRunning);
    pthread_mutex_lock(&sRunning);
    pthread_mutex_lock(&sRunning);

    deconfigureMovement();
    deconfigurePosition();
    deconfigureActionneurs();
    deconfigureIMU();
    deconfigureCF();
    deconfigureIHM();
    deconfigureDebug();
    return EXIT_SUCCESS;
}
