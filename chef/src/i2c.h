#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

void initI2C();
int openI2C(int8_t address);
int8_t readI2C(int fd, int8_t reg);
void writeI2C(int fd, int8_t reg, int8_t data);

// Get full control over the I2C communication
// Note: Don't use methods from this library
// while in this mode
void lockI2C();
void unlockI2C();

#endif
