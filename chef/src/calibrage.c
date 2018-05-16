#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "calibrage.h"

#include "movement.h"
#include "dimensions.h"
#include "securite.h"
#include "motor.h"
#include "common.h"

void calibrer(bool orange)
{
    struct position pos = {0, 0, 0};
    setPosition(&pos);
    return;

    // Calibrage contre mur court
    /* pos.o = orange ? M_PI_2 : -M_PI_2; */
    setDestination(&pos);
    waitDestination();
    disableAsservissement();

    float frontL, frontR, backL, backR;
    struct movAvg diffL, diffR;
    initMovAvg(&diffL, CALIBRAGE_MOVAVG_SIZE);
    initMovAvg(&diffR, CALIBRAGE_MOVAVG_SIZE);
    for (int i = 0; i < CALIBRAGE_TEMPS_ACQUISITION; i += SENSOR_SAMPLING_INTERVAL) {
        getAllDistance(&frontL, &frontR, &backL, &backR);
        addMovAvg(&diffL, frontL - CALIBRAGE_DISTANCE_Y);
        addMovAvg(&diffR, frontR - CALIBRAGE_DISTANCE_Y);
        setMoteurTension(diffL.current * D_KP, diffR.current * D_KP);
        usleep(SENSOR_SAMPLING_INTERVAL * 1000);
    }
    // Les moteurs n'auront peut-être pas atteint le point voulu exactement,
    // on enregistre alors la différence
    float diff = (diffL.current + diffR.current) / 2;
    pos.y = diff;


}
