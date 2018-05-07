#ifndef __CF_H_
#define __CF_H_

#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h> // baudrates
#include <stdbool.h>

#include "CFsignals.h"

#define FPGA_PORTNAME "/dev/ttyUSB0"
#define CF_BAUDRATE B115200
// #define PRINTRAWDATA
//
typedef void (*rxHandler)(void);

void registerRxHandlerCF(unsigned char code, rxHandler handler); // À utiliser après configureCF();
void sendByteCF(unsigned char data); // Privé
void sendCF(unsigned char code, void* data, size_t size);
unsigned char readByteCF(); // À utiliser uniquement depuis un rxHandler
void readCF(void* data, size_t size); // À utiliser uniquement depuis un rxHandler
void configureCF();
void deconfigureCF();

#endif
