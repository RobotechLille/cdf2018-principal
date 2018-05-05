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
#include "ihm.h"
#include "imu.h"
#include "movement.h"
#include "position.h"

pthread_mutex_t sRunning;

#define VIT 1

void endRunning(int signal)
{
    (void)signal;
    pthread_mutex_unlock(&sRunning);
}

int main()
{

    configureDebug();
    configureCF();
    configurePosition();

    changerMoteurs(VIT, VIT);

    long lCod, rCod;
    for (;;) {
        getCoders(&lCod, &rCod);
        printf("%10ld %10ld\n", lCod, rCod);
        usleep(100*1000);
    }

}
