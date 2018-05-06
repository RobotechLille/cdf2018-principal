#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep
#include <wiringPi.h>

#include "CF.h"
#include "debug.h"
#include "i2c.h"
#include "imu.h"
#include "movement.h"
#include "buttons.h"
#include "parcours.h"
#include "position.h"

pthread_mutex_t sRunning;

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
    configurePosition();
    configureMovement();
    startDebug();

    configureParcours();
    prepareParcours(false);
    startParcours();

    int toWait;
    while ((toWait = updateParcours()) >= 0) {
        if (pressedButton(toWait) != none) {
            break;
        }
    }
    stopParcours();

    deconfigureMovement();
    deconfigurePosition();
    deconfigureIMU();
    deconfigureCF();
    deconfigureDebug();
    return EXIT_SUCCESS;
}
