#ifndef __CA_H_
#define __CA_H_

#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h> // baudrates
#include <stdbool.h>

#include "CAsignals.h"

#define ARDUINO_PORTNAME "/dev/ttyACM0"
#define CA_BAUDRATE B9600
// #define PRINTRAWDATA

typedef void (*rxHandler)(void);

void registerRxHandlerCA(unsigned char code, rxHandler handler); // À utiliser après configureCA();
void sendByteCA(unsigned char data); // Privé
void sendCA(unsigned char code, void* data, size_t size);
unsigned char readByteCA(); // À utiliser uniquement depuis un rxHandler
void readCA(void* data, size_t size); // À utiliser uniquement depuis un rxHandler
void configureCA();
void deconfigureCA();

#endif
