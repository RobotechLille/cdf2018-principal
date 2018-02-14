/*
 * Outils assurant la connaissance de la postion du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

#include <FreeRTOS.h>
#include <task.h>

struct position {
    float x;
    float y;
    float o;
};

TaskHandle_t tPosition;

void TaskPosition();
void configurePosition();

#endif
