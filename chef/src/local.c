#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep

#include "CF.h"
#include "debug.h"
#include "movement.h"

pthread_mutex_t sRunning;

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

    /* long lCod, rCod; */
    for (;;) {
        sleep(1);
        /* getCoders(&lCod, &rCod); */
        /* printf("%ld %ld\n", lCod, rCod); */
        /* usleep(100*1000); */
    }

}
