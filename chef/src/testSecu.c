#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "CF.h"
#include "securite.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    configureCF();
    configureSecurite();

    float f, b;

    for (;;) {
        getDistance(&f, &b);
        printf("Av: %6f Ar: %6f\n", f, b);
        sleep(1);
    }
}
