#ifndef __BUTTONS_H_
#define __BUTTONS_H_

#include <stdbool.h>
#include <wiringPi.h>

// Constants
#define BUT_PIN_ROUGE 0
#define BUT_PIN_JAUNE 2
#define BUT_PIN_TIRETTE 3

// ms
#define BUT_BLOCK -1
#define BUT_POLLING_INTERVAL 50
#define BUT_DEBUNK_TIME 50
#define BUT_REFRESH_INTERVAL 1000

// Structures
enum boutons {none, jaune, rouge};

// Public
void configureButtons();
enum boutons pressedButton(int timeout); // timeout: ms or -1
bool tirettePresente();

// Private
bool debunkButton(int pin);
void* TaskStdinButton(void *pdata);

#endif
