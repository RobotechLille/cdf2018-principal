/*
 * Définition des fonctions utilisées pour échager entre l'Arduino et le chef
 */

#ifndef __AF_H_
#define __AF_H_

#include <FreeRTOS.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <queue.h>
#include <semphr.h>
#include <stdlib.h>
#include <task.h>
#include "AFsignals.h"

#define CPU_FREQ 16000000UL
#define AF_PRESCALER CPU_FREQ / (AF_BAUDRATE << 4) - 1

TaskHandle_t tReaderAF;
SemaphoreHandle_t sSendAF;
unsigned char* toSendAF;
size_t toSendSizeAF;

typedef void (*rxHandler)(void);
rxHandler rxHandlersAF[256];

void registerRxHandlerAF(unsigned char code, rxHandler handler); // À utiliser après configureAF();
void sendAF(unsigned char code, void* data, size_t size);
unsigned char readByteAF(); // À utiliser uniquement depuis un rxHandler
void readAF(void* data, size_t size); // À utiliser uniquement depuis un rxHandler
void TaskReaderAF(void* pvParameters); // Privé
void configureAF();

#endif
