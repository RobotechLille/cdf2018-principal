#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "buttons.h"
#include "diagnostics.h"
#include "ihm.h"
#include "lcd.h"
#include "parcours.h"

// Globales
pthread_t tIHM;

// Fonctions
void configureIHM()
{
    initLCD();
    printToLCD(LCD_LINE_1, "Demarrage");
    configureParcours();
}

void startIHM()
{
    configureButtons();
    pthread_create(&tIHM, NULL, TaskIHM, NULL);
}

bool isDebug = false;
bool isOrange = true;
bool annuler = false;
pthread_t tParcours;

char* orangeStr = "Orange";
char* vertStr = "Vert";

char* getCouleur()
{
    return isOrange ? orangeStr : vertStr;
}

struct timespec calibrageLast = { 0, 0 };
struct timespec calibrageNow;
struct timespec calibrageEcoule;

void* TaskIHM(void* pdata)
{
    (void)pdata;

    enum boutons bout;
    for (;;) {

        /* // Debug */
        /* for (;;) { */
        /*     clearLCD(); */
        /*     printfToLCD(LCD_LINE_1, "Debug : %s", isDebug ? "On" : "Off"); */
        /*     if (isDebug) { */
        /*         printToLCD(LCD_LINE_2, "192.168.0.0 TODO"); */
        /*     } */
        /*     bout = pressedButton(BUT_REFRESH_INTERVAL); */
        /*  */
        /*     if (bout == rouge) { */
        /*         isDebug = !isDebug; */
        /*     } else if (bout == jaune) { */
        /*         break; */
        /*     } */
        /* } */

        // Diagnostics
        for (;;) {
            clearLCD();
            printToLCD(LCD_LINE_1, "Diagnostiquer");
            bout = pressedButton(BUT_BLOCK);

            if (bout == rouge) {
                clearLCD();
                runDiagnostics();
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
            if (calibrageLast.tv_sec > 0) {
                clock_gettime(CLOCK_REALTIME, &calibrageNow);
                if ((calibrageNow.tv_nsec - calibrageLast.tv_nsec) > 0) {
                    calibrageEcoule.tv_sec = calibrageNow.tv_sec - calibrageLast.tv_sec - 1;
                    calibrageEcoule.tv_nsec = calibrageNow.tv_nsec - calibrageLast.tv_nsec + 1000000000UL;
                } else {
                    calibrageEcoule.tv_sec = calibrageNow.tv_sec - calibrageLast.tv_sec;
                    calibrageEcoule.tv_nsec = calibrageNow.tv_nsec - calibrageLast.tv_nsec;
                }
                printToLCD(LCD_LINE_1, "Calibre il y a");
                printfToLCD(LCD_LINE_2, "%ld secondes", calibrageEcoule.tv_sec);
            } else {
                printToLCD(LCD_LINE_1, "Calibrer");
                printfToLCD(LCD_LINE_2, "(%s)", getCouleur());
            }
            bout = pressedButton(BUT_REFRESH_INTERVAL);

            if (bout == rouge) {
                clearLCD();
                printToLCD(LCD_LINE_1, "Calibrage...");
                delay(3000); // TODO
                clock_gettime(CLOCK_REALTIME, &calibrageLast);
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
    pthread_cancel(tIHM);
    clearLCD();
    printToLCD(LCD_LINE_1, "Bye bye!");
}
