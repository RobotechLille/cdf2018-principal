/*
 * Fonctions de calcul de la position du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

#include "CF.h"
#include <pthread.h>


struct __attribute__ ((packed)) position {
    float x;
    float y;
    float o;
};

struct position actuel;
struct F2CI_CODERs deltaCoders;

// Debug
unsigned int nbCalcPos;
long lCodTot, rCodTot;

// 
pthread_mutex_t posPolling;
pthread_t tPosition;

// Fonctions
void configurePosition();
void deconfigurePosition();

#endif

