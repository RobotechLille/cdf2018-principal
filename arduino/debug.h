/*
 * Outils pour envoyer des informations de debug au chef
 */

#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <FreeRTOS.h>
#include <task.h>

TaskHandle_t tDebug;

struct A2CI_DBGs debug;

void TaskDebug();
void configureDebug();

#endif
