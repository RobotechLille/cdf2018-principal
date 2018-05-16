#ifndef __COMMON_H_
#define __COMMON_H_

#include <time.h>
#include <stdlib.h>

void diffTime(const struct timespec* debut, const struct timespec* fin, struct timespec* ecoule);
float diffTimeSec(const struct timespec* debut, const struct timespec* fin);

// PID
struct PID {
    struct timespec lastCalc;
    float KP;
    float KI;
    float KD;
    float prevErr;
    float integErr;
};

void resetPID(struct PID *pid);
void initPID(struct PID *pid, float KP, float KI, float KD);
float updatePID(struct PID *pid, float err);

// Moving average
struct movAvg {
    size_t size;
    size_t actuel;
    float* table;
    double total;
    float current;
};

void initMovAvg(struct movAvg *movavg, size_t size);
void addMovAvg(struct movAvg *movavg, float val);


#endif
