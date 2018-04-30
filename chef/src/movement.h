/*
 * Fonctions de déplacment du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#include "position.h"

#include <wiringPi.h>
#define PWM_MAX 1023
#define PWM_MAX_V 3.3

// #define TESTINATOR
#define TLE5206

#ifdef TESTINATOR
#define MOT_MIN_V 0.1
#define MOT_MAX_V 2.0
#endif

#ifdef TLE5206
#define MOT_MIN_V 0.1
#define MOT_MAX_V 2.5
#endif

// Pins definition
// Left
// Physical 32
#define ENA 26
#define IN1 2
#define IN2 3

// Physical 33
// Right
#define ENB 23
#define IN3 4
#define IN4 5


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
