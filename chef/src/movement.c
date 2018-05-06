#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CF.h"
#include "dimensions.h"
#include "motor.h"
#include "movement.h"

void configureMovement()
{
    stop();
}

void aller(struct position* pos);

void deconfigureMovement()
{
    stop();
}
