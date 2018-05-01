#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "ihm.h"
#include "movement.h"
#include "parcours.h"
#include "points.h"
#include "lcd.h"
#include "buttons.h"

// Globales
pthread_t tIHM;

// Fonctions
void configureIHM()
{
    initLCD();
    printToLCD(LCD_LINE_1, "Demarrage");
}

void startIHM()
{
    configureButtons();
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

bool isDebug = false;
bool isOrange = true;
bool annuler = false;
clock_t lastCalibrage = 0;
pthread_t tParcours;

char* orangeStr = "Orange";
char* vertStr = "Vert";

char* getCouleur()
{
    return isOrange ? orangeStr : vertStr;
}

void* TaskIHM(void* pdata)
{
    (void)pdata;

    enum boutons bout;
    for (;;) {

        // Debug
        for (;;) {
            clearLCD();
            printfToLCD(LCD_LINE_1, "Debug : %s", isDebug ? "On" : "Off");
            if (isDebug) {
                printToLCD(LCD_LINE_2, "192.168.0.0 TODO");
            }
            bout = pressedButton(BUT_REFRESH_INTERVAL);

            if (bout == rouge) {
                isDebug = !isDebug;
            } else if (bout == jaune) {
                break;
            }
        }

        // Couleur
        for (;;) {
            clearLCD();
            printfToLCD(LCD_LINE_1, "Couleur : %s", getCouleur());
            bout = pressedButton(BUT_BLOCK);

            if (bout == rouge) {
                isOrange = !isOrange;
            } else if (bout == jaune) {
                break;
            }
        }

        // Calibrage
        for (;;) {
            clearLCD();
            if (lastCalibrage != 0) {
                printToLCD(LCD_LINE_1, "Calibre il y a");
                printfToLCD(LCD_LINE_2, "%ld secondes", (clock() - lastCalibrage) / CLOCKS_PER_SEC);
            } else {
                printToLCD(LCD_LINE_1, "Calibrer");
                printfToLCD(LCD_LINE_2, "(%s)", getCouleur());
            }
            bout = pressedButton(BUT_REFRESH_INTERVAL);

            if (bout == rouge) {
                clearLCD();
                printToLCD(LCD_LINE_1, "Calibrage...");
                delay(3000);             // TODO
                lastCalibrage = clock(); // TODO struct timespec
            } else if (bout == jaune) {
                break;
            }
        }

        // Diagnostics
        for (;;) {
            clearLCD();
            printToLCD(LCD_LINE_1, "Diagnostiquer");
            bout = pressedButton(BUT_BLOCK);

            if (bout == rouge) {
                clearLCD();
                printToLCD(LCD_LINE_1, "Diagnostics...");
                delay(3000); // TODO
            } else if (bout == jaune) {
                break;
            }
        }

        // Parcours
        for (;;) {
            clearLCD();
            printToLCD(LCD_LINE_1, "Lancer parcours");
            printfToLCD(LCD_LINE_2, "(%s)", getCouleur());
            bout = pressedButton(BUT_BLOCK);
            if (bout == rouge) {
                // No tirette
                annuler = false;
                while (!tirettePresente()) {
                    clearLCD();
                    printToLCD(LCD_LINE_1, "Inserez tirette");
                    printToLCD(LCD_LINE_2, "(ROUGE: ignorer)");
                    bout = pressedButton(BUT_REFRESH_INTERVAL);
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
                    bout = pressedButton(BUT_POLLING_INTERVAL);
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
                    if (pressedButton(toWait) != none) {
                        break;
                    }
                }
                stopParcours();

                pressedButton(BUT_BLOCK); // Nécessite 3 appuis pour éviter d'enlever le score par inadvertance
                pressedButton(BUT_BLOCK);
                pressedButton(BUT_BLOCK);
            } else if (bout == jaune) {
                break;
            }
        }

        // RàZ
        for (;;) {
            clearLCD();
            printToLCD(LCD_LINE_1, "Remettre a zero");
            bout = pressedButton(BUT_BLOCK);

            if (bout == rouge) {
                clearLCD();
                printToLCD(LCD_LINE_1, "Remise a zero...");
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
    printToLCD(LCD_LINE_1, "Bye bye!");
}
