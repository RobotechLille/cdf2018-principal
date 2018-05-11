#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

#include "i2c.h"
#include "securite.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    initI2C();
    configureSecurite();

    float fl, fr, bl, br;

    for (;;) {
        getAllDistance(&fl, &fr, &bl, &br);
        printf("FL: %9f FR: %9f BL: %9f BR: %9f\n", fl, fr, bl, br);
        usleep(60 * 1000);
    }
}
