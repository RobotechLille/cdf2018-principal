#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <math.h>
#include <stdint.h>

#include "CF.h"
#include "dimensions.h"

#define INVERSE_L_MOTOR
// #define INVERSE_R_MOTOR

// V/s
#define RATE_LIMITER_UP 6
#define RATE_LIMITER_DOWN 1

#define TESTINATOR
// #define TLE5206

#define IN1 0
#define IN2 1
#define IN3 2
#define IN4 3

enum motorState {
    running,
    braking,
    freewheeling
};

// Public
void configureMotor();
void deconfigureMotor();
void setMoteurTension(float lVolt, float rVolt);
void setPWMTension(float lVolt, float rVolt);
int brake();
int freewheel();
int stop();

// Private
void* TaskMotor(void* pData);
uint8_t moteurTensionToPWM(float V);
void setMoteurTensionRaw(float lVolt, float rVolt, bool lFor, bool rFor);
void rawFreewheel();
void rawBrake();

#endif
