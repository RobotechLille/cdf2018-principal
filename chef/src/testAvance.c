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
#include "motor.h"
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
    configureCF();
    configureIMU();
    configureActionneurs();
    configurePosition();
    configureMovement();
    startDebug();

    debugSetActive(true);
    sleep(1);
    /* struct position pos = {350, 0, -0.95*M_PI/3.0 }; */
    struct position pos = {100000, 0, 0 };
    setDestination(&pos);
    waitDestination();
    for (;;) {
        setLoquet(false);
        setLoquet(true);
    }
    printf("Done\n");

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
    deconfigureDebug();
    return EXIT_SUCCESS;
}
