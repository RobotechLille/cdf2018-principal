#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // sleep

#include "debug.h"

unsigned long int canard = 42;
double banane = 63;
char* artichaut = "Torticoli";

int main()
{

    printf("Démarrage...\n");
    srand(time(NULL));

    configureDebug();
    registerDebugVar("canard", ld, &canard);
    registerDebugVar("banane", lf, &banane);
    registerDebugVar("artichaut", s, &artichaut);
    startDebug();

    for (int i = 0; i < 2; i++) {
        printf("21 %d\n", i);
        canard += 3;
        banane /= 2;
        sleep(1);
    }

    deconfigureDebug();


    return EXIT_SUCCESS;
}

