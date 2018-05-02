/*
 * Fonctions de calcul de la position du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

#include "CF.h"
#include <pthread.h>


// Structures
struct __attribute__ ((packed)) position {
    float x;
    float y;
    float o;
};

// Fonctions
void configurePosition();
void deconfigurePosition();
void getCoders(long* l, long* r);

#endif

