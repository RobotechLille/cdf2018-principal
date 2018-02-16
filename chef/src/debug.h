#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <pthread.h>
#include "CA.h"
#define REGULARREPORTS

struct A2CI_DBGs debug;
pthread_t tDebug;

void* TaskDebug(void *pdata);
void onA2CI_DBG();
void configureDebug();
void deconfigureDebug();

#endif
