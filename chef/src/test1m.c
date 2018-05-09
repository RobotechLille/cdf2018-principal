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
    printf("40\n");
    configureActionneurs();
    printf("41\n");
    configurePosition();
    printf("44\n");
    configureMovement();
    printf("46\n");
    startDebug();
    printf("48\n");

    debugSetActive(true);
    sleep(1);
    printf("46\n");
    enableConsigne();
    struct position pos = { 0, -100, -M_PI };
    setDestination(&pos);
    printf("50\n");
    waitDestination();
    printf("52\n");
    disableConsigne();
    printf("54\n");
    brake();
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
