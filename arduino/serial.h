#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <semphr.h>
#include <stdlib.h>
#include <task.h>

#define CPU_FREQ 16000000UL
#define AC_PRESCALER CPU_FREQ / (AC_BAUDRATE << 4) - 1

TaskHandle_t tReaderAC;
SemaphoreHandle_t sSendAC;

void sendByteAC(unsigned char data);
void sendAC(unsigned char code, void* data, size_t size);
unsigned char readAC();
void TaskReaderAC(void *pvParameters);
void configureAC();

#endif
