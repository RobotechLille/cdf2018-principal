#include "common.h"

void diffTime(const struct timespec* debut, const struct timespec* fin, struct timespec* ecoule)
{
    if ((fin->tv_nsec - debut->tv_nsec) < 0) {
        ecoule->tv_sec = fin->tv_sec - debut->tv_sec - 1;
        ecoule->tv_nsec = fin->tv_nsec - debut->tv_nsec + 1000000000UL;
    } else {
        ecoule->tv_sec = fin->tv_sec - debut->tv_sec;
        ecoule->tv_nsec = fin->tv_nsec - debut->tv_nsec;
    }
}

float diffTimeSec(const struct timespec* debut, const struct timespec* fin)
{
    struct timespec ecoule;
    diffTime(debut, fin, &ecoule);
    return ecoule.tv_sec + ecoule.tv_nsec * 1E-9;
}

void resetPID(struct PID *pid)
{
    clock_gettime(CLOCK_REALTIME, &pid->lastCalc);
    pid->prevErr = 0;
    pid->integErr = 0;
}

void initPID(struct PID *pid, float KP, float KI, float KD)
{
    pid->KP = KP;
    pid->KI = KI;
    pid->KD = KD;
    resetPID(pid);
}

float updatePID(struct PID *pid, float err)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    float dt = diffTimeSec(&pid->lastCalc, &now);
    pid->integErr += (err + pid->prevErr) / 2 * dt;
    float derivErr = (err - pid->prevErr) / dt;
    pid->prevErr = err;
    return pid->KP * err + pid->KI * pid->integErr + pid->KP * derivErr;
}

void initMovAvg(struct movAvg *movavg, size_t size)
{
    movavg->size = size;
    movavg->actuel = 0;
    movavg->table = malloc(movavg->size * sizeof(float));
    for (size_t i = 0; i < movavg->size; i++) {
        movavg->table[i] = 0.0;
    }
    movavg->total = 0.0;
}

void addMovAvg(struct movAvg *movavg, float val)
{
    movavg->total -= movavg->table[movavg->actuel];
    movavg->table[movavg->actuel] = val;
    movavg->total += movavg->table[movavg->actuel];
    movavg->actuel++;
    movavg->current = movavg->total / (double) movavg->size;
    if (movavg->actuel >= movavg->size) {
        movavg->actuel = 0;
    }
}
