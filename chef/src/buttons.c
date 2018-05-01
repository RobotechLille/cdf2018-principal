#include <pthread.h>
#include <stdio.h>

#include "buttons.h"

// Globales
pthread_t tStdinButton;
enum boutons stdinbutton = none;

// Fonctions
void configureButtons()
{
    pinMode(BUT_PIN_ROUGE, INPUT);
    pullUpDnControl(BUT_PIN_ROUGE, PUD_UP);
    pinMode(BUT_PIN_JAUNE, INPUT);
    pullUpDnControl(BUT_PIN_JAUNE, PUD_UP);
    pinMode(BUT_PIN_TIRETTE, INPUT);
    pullUpDnControl(BUT_PIN_TIRETTE, PUD_UP);

    pthread_create(&tStdinButton, NULL, TaskStdinButton, NULL);
}

bool debunkButton(int pin)
{
    int t;
    // Press (cancel if wrong)
    for (t = BUT_DEBUNK_TIME / 2; t > 0; t--) {
        if (digitalRead(pin) != LOW) {
            return false;
        }
        delay(1);
    }
    // Release (re-wait if wrong)
    for (t = BUT_DEBUNK_TIME / 2; t > 0; t--) {
        if (digitalRead(pin) != HIGH) {
            t = BUT_DEBUNK_TIME / 2;
        }
        delay(1);
    }
    return true;
}


void* TaskStdinButton(void* pdata)

{
    (void)pdata;

    for (;;) {
        char c = getchar();
        if (c == '1') {
            stdinbutton = jaune;
        } else if (c == '2') {
            stdinbutton = rouge;
        }
    }

    return NULL;
}

enum boutons pressedButton(int timeout)
{
    bool block = timeout < 0;
    while (timeout > 0 || block) {

        if (stdinbutton != none) {
            enum boutons bout = stdinbutton;
            stdinbutton = none;
            return bout;
        }

        if (debunkButton(BUT_PIN_JAUNE)) {
            return jaune;
        }

        if (debunkButton(BUT_PIN_ROUGE)) {
            return rouge;
        }

        delay(BUT_POLLING_INTERVAL);
        timeout -= BUT_POLLING_INTERVAL;
    }
    return none;
}

bool tirettePresente()
{
    int etat, newEtat;
    int t;
    for (t = 0; t < BUT_DEBUNK_TIME; t++) {
        newEtat = digitalRead(BUT_PIN_TIRETTE);
        if (etat != newEtat) {
            t = 0;
            etat = newEtat;
        }
        delay(1);
    }
    return etat == LOW;
}
