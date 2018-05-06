#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <math.h>
#include <stdint.h>

#include "dimensions.h"
#include "CF.h"

#define TESTINATOR
// #define TLE5206

#define IN1 0
#define IN2 1
#define IN3 2
#define IN4 3

// Public
void setMoteurTension(float lVolt, float rVolt);
void setPWMTension(float lVolt, float rVolt);
int brake();
int freewheel();
int stop();

// Private
uint8_t moteurTensionToPWM(float V);
void setMoteurTensionRaw(float lVolt, float rVolt, bool lFor, bool rFor);

#endif
