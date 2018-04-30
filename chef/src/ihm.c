#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <wiringPi.h>

#include "ihm.h"
#include "movement.h"
#include "parcours.h"
#include "points.h"

// Globales
pthread_t tIHM;

// Fonctions
void configureIHM()
{
    initLCD();
    gotoLCD(LCD_LINE_1);
    printLCD("Demarrage...");
}

void startIHM()
{
    pinMode(IHM_PIN_ROUGE, INPUT);
    pullUpDnControl(IHM_PIN_ROUGE, PUD_UP);
    pinMode(IHM_PIN_JAUNE, INPUT);
    pullUpDnControl(IHM_PIN_JAUNE, PUD_UP);
    pinMode(IHM_PIN_TIRETTE, INPUT);
    pullUpDnControl(IHM_PIN_TIRETTE, PUD_UP);

    pthread_create(&tIHM, NULL, TaskIHM, NULL);
    pthread_join(tIHM, NULL);
}

// t1 - t2
void diffTimespec(const struct timespec* t1, const struct timespec* t2, struct timespec* td)
{
    if ((t1->tv_nsec - t2->tv_nsec) < 0) {
        td->tv_sec = t1->tv_sec - t2->tv_sec - 1;
        td->tv_nsec = t1->tv_nsec - t2->tv_nsec + 1000000000UL;
    } else {
        td->tv_sec = t1->tv_sec - t2->tv_sec;
        td->tv_nsec = t1->tv_nsec - t2->tv_nsec;
    }
}

bool debunkButtonIHM(int pin)
{
    int t;
    // Press (cancel if wrong)
    for (t = IHM_DEBUNK_TIME / 2; t > 0; t--) {
        if (digitalRead(pin) != LOW) {
            return false;
        }
        delay(1);
    }
    // Release (re-wait if wrong)
    for (t = IHM_DEBUNK_TIME / 2; t > 0; t--) {
        if (digitalRead(pin) != HIGH) {
            t = IHM_DEBUNK_TIME / 2;
        }
        delay(1);
    }
    return true;
}

enum boutons pressedIHM(int timeout)
{
    bool block = timeout < 0;
    while (timeout > 0 || block) {

        if (debunkButtonIHM(IHM_PIN_JAUNE)) {
            return jaune;
        }

        if (debunkButtonIHM(IHM_PIN_ROUGE)) {
            return rouge;
        }

        delay(IHM_POLLING_INTERVAL);
        timeout -= IHM_POLLING_INTERVAL;
    }
    return none;
}

bool tirettePresente()
{
    int etat, newEtat;
    int t;
    for (t = 0; t < IHM_DEBUNK_TIME; t++) {
        newEtat = digitalRead(IHM_PIN_TIRETTE);
        if (etat != newEtat) {
            t = 0;
            etat = newEtat;
        }
        delay(1);
    }
    return etat == LOW;
}

char tempLine[16];
bool isDebug = false;
bool isOrange = true;
bool annuler = false;
clock_t lastCalibrage = 0;
clock_t parcoursStart = 0;
pthread_t tParcours;

void printCouleur()
{
    if (isOrange) {
        printLCD("Orange");
    } else {
        printLCD("Vert");
    }
}

void* TaskIHM(void* pdata)
{
    (void)pdata;

    gotoLCD(LCD_LINE_1 + 1);
    printLCD("Niuh");

    enum boutons bout;
    for (;;) {

        // Debug
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            printLCD("Debug : ");
            if (isDebug) {
                printLCD("On");
                gotoLCD(LCD_LINE_2);
                printLCD("192.168.0.0 TODO");
            } else {
                printLCD("Off");
            }
            bout = pressedIHM(IHM_REFRESH_INTERVAL);

            if (bout == rouge) {
                isDebug = !isDebug;
            } else if (bout == jaune) {
                break;
            }
        }

        // Couleur
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            printLCD("Couleur : ");
            printCouleur();
            bout = pressedIHM(IHM_BLOCK);

            if (bout == rouge) {
                isOrange = !isOrange;
            } else if (bout == jaune) {
                break;
            }
        }

        // Calibrage
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            if (lastCalibrage != 0) {
                printLCD("Calibre il y a");
                gotoLCD(LCD_LINE_2);
                sprintf(tempLine, "%ld secondes", (clock() - lastCalibrage) / CLOCKS_PER_SEC);
                printLCD(tempLine);
            } else {
                printLCD("Calibrer");
                gotoLCD(LCD_LINE_2);
                printLCD("(");
                printCouleur();
                printLCD(")");
            }
            bout = pressedIHM(IHM_REFRESH_INTERVAL);

            if (bout == rouge) {
                clearLCD();
                gotoLCD(LCD_LINE_1);
                printLCD("Calibrage...");
                delay(3000); // TODO
                lastCalibrage = clock();
            } else if (bout == jaune) {
                break;
            }
        }

        // Diagnostics
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            printLCD("Diagnostiquer");
            bout = pressedIHM(IHM_BLOCK);

            if (bout == rouge) {
                clearLCD();
                gotoLCD(LCD_LINE_1);
                printLCD("Diagnostics...");
                delay(3000); // TODO
            } else if (bout == jaune) {
                break;
            }
        }

        // Parcours
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            printLCD("Lancer parcours");
            gotoLCD(LCD_LINE_2);
            printLCD("(");
            printCouleur();
            printLCD(")");
            bout = pressedIHM(IHM_BLOCK);
            if (bout == rouge) {
                // No tirette
                annuler = false;
                while (!tirettePresente()) {
                    clearLCD();
                    gotoLCD(LCD_LINE_1);
                    printLCD("Inserez tirette");
                    gotoLCD(LCD_LINE_2);
                    printLCD("(ROUGE: ignorer)");
                    bout = pressedIHM(IHM_REFRESH_INTERVAL);
                    if (bout == rouge) {
                        break;
                    } else if (bout == jaune) {
                        annuler = true;
                        break;
                    }
                }

                if (annuler) {
                    continue;
                }

                // Go!

                prepareParcours(isOrange);
                while (tirettePresente()) {
                    bout = pressedIHM(IHM_POLLING_INTERVAL);
                    if (bout == jaune) {
                        annuler = true;
                        break;
                    }
                }

                if (annuler) {
                    continue;
                }

                startParcours(); // TODO On a different thread

                int toWait;
                while ((toWait = updateParcours()) >= 0) {
                    if (pressedIHM(toWait) != none) {
                        break;
                    }
                }
                stopParcours();

                pressedIHM(IHM_BLOCK); // Nécessite 3 appuis pour éviter d'enlever le score par inadvertance
                pressedIHM(IHM_BLOCK);
                pressedIHM(IHM_BLOCK);
            } else if (bout == jaune) {
                break;
            }
        }

        // RàZ
        for (;;) {
            clearLCD();
            gotoLCD(LCD_LINE_1);
            printLCD("Remettre a zero");
            bout = pressedIHM(IHM_BLOCK);

            if (bout == rouge) {
                clearLCD();
                gotoLCD(LCD_LINE_1);
                printLCD("Remise a zero...");
                delay(3000); // TODO
            } else if (bout == jaune) {
                break;
            }
        }
    }
}

void deconfigureIHM()
{
    clearLCD();
    gotoLCD(LCD_LINE_1);
    printLCD("Bye bye!");
}
