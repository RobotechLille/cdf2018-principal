/*
 * Outils assurant la connaissance de la postion du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

#include <FreeRTOS.h>
#include <task.h>

#include "ACsignals.h"
#include "AFsignals.h"

struct position actuel;
struct F2AI_CODERs deltaCoder;

TaskHandle_t tPosition;

uint16_t nbCalcPos;

void TaskPosition();
void configurePosition();

#endif
