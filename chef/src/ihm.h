#ifndef __IHM_H_
#define __IHM_H_

#include "lcd.h"

#define IHM_PIN_ROUGE 0
#define IHM_PIN_JAUNE 2
#define IHM_PIN_TIRETTE 3

// ms
#define IHM_BLOCK -1
#define IHM_POLLING_INTERVAL 50
#define IHM_DEBUNK_TIME 50
#define IHM_REFRESH_INTERVAL 1000

enum boutons {none, jaune, rouge};

// Public
void configureIHM();
void startIHM();
void deconfigureIHM();

// Private
void* TaskIHM(void *pdata);
enum boutons pressedIHM(int timeout); // timeout: ms or -1

#endif
