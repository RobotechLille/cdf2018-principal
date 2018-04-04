#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <pthread.h>
#include <time.h>

clock_t debugStart;
int debugFd;

pthread_t tDebug;

void* TaskDebug(void *pdata);
void configureDebug();
void deconfigureDebug();

#endif
