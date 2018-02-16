/*
 * Définition des fonctions utilisées pour échager entre l'Arduino et le chef
 */

#ifndef __AC_H_
#define __AC_H_

#include <FreeRTOS.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <queue.h>
#include <semphr.h>
#include <stdlib.h>
#include <task.h>
#include "ACsignals.h"

#define CPU_FREQ 16000000UL
#define AC_PRESCALER CPU_FREQ / (AC_BAUDRATE << 4) - 1

TaskHandle_t tReaderAC;
SemaphoreHandle_t sSendAC;
unsigned char* toSendAC;
size_t toSendSizeAC;

typedef void (*rxHandler)(void);
rxHandler rxHandlersAC[256];

void registerRxHandlerAC(unsigned char code, rxHandler handler); // À utiliser après configureAC();
void sendAC(unsigned char code, void* data, size_t size);
unsigned char readByteAC(); // À utiliser uniquement depuis un rxHandler
void readAC(void* data, size_t size); // À utiliser uniquement depuis un rxHandler
void TaskReaderAC(void* pvParameters); // Privé
void configureAC();

#endif
