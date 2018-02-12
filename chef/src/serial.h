#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <sys/ioctl.h>
#include <termios.h> // baudrates

#include "ACsignals.h"

#define ARDUINO_PORTNAME "/dev/ttyACM0"
#define CA_BAUDRATE B9600

int arduino;

void configureCA();
void deconfigureCA();
void sendByteCA(unsigned char data);
unsigned char readByteCA();

#endif
