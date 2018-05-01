#ifndef __IHM_H_
#define __IHM_H_

#include <stdbool.h>

// Public
void configureIHM();
void startIHM();
void deconfigureIHM();

// Private
void* TaskIHM(void *pdata);

#endif
