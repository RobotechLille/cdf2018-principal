/*
 * Fonctions de déplacment du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#include "position.h"

#define PWM_MAX 255
#define PWM_MAX_V 3.3

#define TESTINATOR
// #define TLE5206

#ifdef TESTINATOR
#define MOT_MIN_V 0.1
#define MOT_MAX_V 2.0
#endif

#ifdef TLE5206
#define MOT_MIN_V 0.1
#define MOT_MAX_V 2.5
#endif

#define IN1 0
#define IN2 1
#define IN3 2
#define IN4 3

void configureMovement();
void aller(struct position* pos);
int changerMoteurs(float vitL, float vitR);
// Vitesse en mm/s
// Vitesse < 0 ⇒ sens inverse
// Si vitesse < seuil ⇒ brake
int brake();
int stop();
int freewheel();
void deconfigureMovement();

#endif
