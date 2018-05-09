#include <stdlib.h>
#include <unistd.h>

#include "CF.h"
#include "motor.h"
#include "debug.h"

int main(int argc, char* argv[])
{

    (void)argc;
    (void)argv;

    configureDebug();
    configureCF();
    configureMotor();

    setMoteurTension(24, 24);
    sleep(10);
    setMoteurTension(0, 0);
    sleep(3);

    deconfigureMotor();
    deconfigureCF();
    deconfigureDebug();
}
