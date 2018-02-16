#ifndef __AC_H_
#define __AC_H_

#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h> // baudrates
#include <stdbool.h>

#include "ACsignals.h"

#define ARDUINO_PORTNAME "/dev/ttyACM0"
#define CA_BAUDRATE B9600
// #define PRINTRAWDATA

int arduino;
pthread_mutex_t sSendCA;
pthread_t tReaderAC;

typedef void (*rxHandler)(void);
rxHandler rxHandlersAC[256];
bool pret;

void registerRxHandler(unsigned char code, rxHandler handler); // À utiliser après configureCA();
void sendByteCA(unsigned char data); // Privé
void sendCA(unsigned char code, void* data, size_t size);
unsigned char readByteCA(); // À utiliser uniquement depuis un rxHandler
void readCA(void* data, size_t size); // À utiliser uniquement depuis un rxHandler
void configureCA();
void deconfigureCA();

#endif
