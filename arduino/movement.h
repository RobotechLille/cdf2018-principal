/*
 * Outils assurant le déplacement du robot
 */

#ifndef __MOVEMENT_H_
#define __MOVEMENT_H_

#include <FreeRTOS.h>
#include <task.h>
#include "stdbool.h"
#include "ACsignals.h"

// TODO Définition des pins

TaskHandle_t tMovement;

unsigned char movement;
struct C2AD_GOTOs destination;

// Mouvements qui ne dérangent pas la liaison série
void brake();
void freewheel();
void stop();

void TaskMovement();
void configureMovement();

#endif
