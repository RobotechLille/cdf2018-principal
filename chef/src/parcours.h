#ifndef __PARCOURS_H__
#define __PARCOURS_H__

#include <stdbool.h>
#include <math.h>

#include "movement.h"

#define TEMPS_PARCOURS 100
#define NB_BALLES 8

#define X_RECUP_1 1160
#define Y_RECUP_1 210
#define O_RECUP_1 -M_PI

#define X_LANCER X_RECUP_1
#define Y_LANCER Y_RECUP_1 + 100
#define O_LANCER 3 * M_PI_4

#define X_RECUP_2 610
#define Y_RECUP_2 50
// ↑
#define O_RECUP_2 -M_PI_2

#define X_EVIT X_RECUP_2
#define Y_EVIT 1000
#define O_EVIT ANGLE_INSIGNIFIANT

#define X_ABEILLE 210
#define Y_ABEILLE 50
// ↑
#define O_ABEILLE -M_PI_2

#define X_USE 800
// ↑
#define Y_USE 100
// ↑
#define O_USE 0


void configureParcours();
void prepareParcours(bool orange);
void startParcours();
// Returns : -1 if parcours ended, N ms for the next time it should be checked
int updateParcours();
void stopParcours();
void* TaskParcours(void* pdata);

#endif
