#ifndef __COMMON_H_
#define __COMMON_H_

#include <time.h>

struct PID {
    struct timespec lastCalc;
    float KP;
    float KI;
    float KD;
    float prevErr;
    float integErr;
};

void diffTime(const struct timespec* debut, const struct timespec* fin, struct timespec* ecoule);
float diffTimeSec(const struct timespec* debut, const struct timespec* fin);
void resetPID(struct PID *pid);
void initPID(struct PID *pid, float KP, float KI, float KD);
float updatePID(struct PID *pid, float err);

#endif
