/*
 * Fonctions de déplacment du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#define ANGLE_INSIGNIFIANT NAN

// #define ENABLE_SECURITE

#include "position.h"

// Public
void configureMovement();
void deconfigureMovement();
void setDestination(struct position* pos);
void waitDestination();
void enableAsservissement();
void disableAsservissement();

// Private
void* TaskMovement(void* pData);

#endif
