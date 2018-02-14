/*
 * Outils assurant la connaissance de la postion du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

#include <FreeRTOS.h>
#include <task.h>

#include "ACsignals.h"

struct position actuel;

TaskHandle_t tPosition;

void TaskPosition();
void configurePosition();

#endif
