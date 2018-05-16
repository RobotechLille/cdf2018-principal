/*
 * Fonctions de déplacment du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#define ANGLE_INSIGNIFIANT NAN

// #define ENABLE_SECURITE

#include <stdbool.h>

#include "position.h"

enum movStates {
    quelconque,  // 0
    direction,   // 1
    approche,    // 2
    arret,       // 3
    orientation, // 4
    oriente,     // 5
    fini         // 6
};

// Public
void configureMovement();
void deconfigureMovement();
void setDestination(struct position* pos);
void waitDestination();
void enableAsservissement();
void disableAsservissement();
void setSecurite(bool av, bool ar);

// Private
void* TaskMovement(void* pData);

#endif
