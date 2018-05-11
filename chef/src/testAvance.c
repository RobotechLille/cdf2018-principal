#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "actionneurs.h"
#include "debug.h"
#include "i2c.h"
#include "ihm.h"
#include "motor.h"
#include "movement.h"
#include "position.h"

pthread_mutex_t sRunning;

void endRunning(int signal)
{
    (void)signal;
    pthread_mutex_unlock(&sRunning);
}

int main(int argc, char* argv[])
{

    if (wiringPiSetup() < 0) {
        fprintf(stderr, "Impossible d'initialiser WiringPi\n");
        exit(EXIT_FAILURE);
    }
    initI2C();
    srand(time(NULL));

    float x = 0;
    float y = 0;
    float o = 0;
    if (argc >= 2) {
        sscanf(argv[1], "%f", &x);
        if (argc >= 3) {
            sscanf(argv[2], "%f", &y);
            if (argc >= 4) {
                sscanf(argv[3], "%f", &o);
            }
        }
    }

    configureDebug();
    configurePosition();
    configureMovement();
    debugSetActive(true);
    startDebug();

    struct position pos = { x, y, o };
    printf("Go\n");
    setDestination(&pos);
    enableAsservissement();
    waitDestination();
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
    deconfigureDebug();
    return EXIT_SUCCESS;
}
