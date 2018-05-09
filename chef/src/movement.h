/*
 * Fonctions de déplacment du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#define ANGLE_INSIGNIFIANT NAN

// #define ENABLE_SECURITE

#include "position.h"

void configureMovement();
void deconfigureMovement();
void setDestination(struct position* pos);
void* TaskMovement(void* pData);
void enableConsigne();
void disableConsigne();
void waitDestination();

#endif
