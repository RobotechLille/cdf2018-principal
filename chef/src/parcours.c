#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#include "lcd.h"
#include "movement.h"
#include "parcours.h"
#include "points.h"
#include "position.h"
#include "debug.h"

pthread_t tParcours;
bool isOrange;
struct timespec tempsStart;
struct timespec tempsNow;
struct timespec tempsEcoule = {0, 0};

void configureParcours()
{
    registerDebugVar("temps", ld, &tempsEcoule.tv_sec);
    configurePoints();
}

void prepareParcours(bool orange)
{
    isOrange = orange;
    clearLCD();
    printfToLCD(LCD_LINE_1, "--:--/%2d:%02d", TEMPS_PARCOURS / 60, TEMPS_PARCOURS % 60);
    printRightLCD(LCD_LINE_1, "ATT");

    resetPoints();
    showPoints();
    printRightLCD(LCD_LINE_2, isOrange ? "Org" : "Vrt");
}

void startParcours()
{
    clock_gettime(CLOCK_REALTIME, &tempsStart);
    pthread_create(&tParcours, NULL, TaskParcours, NULL); // TODO Start on mutex unlock
    printRightLCD(LCD_LINE_1, "   ");
    debugSetActive(true);
}

void updateTimeDisplay()
{
    printfToLCD(LCD_LINE_1, "%2ld:%02ld", tempsEcoule.tv_sec / 60, tempsEcoule.tv_sec % 60);
}

int updateParcours()
{
    clock_gettime(CLOCK_REALTIME, &tempsNow);
    if ((tempsNow.tv_nsec - tempsStart.tv_nsec) < 0) {
        tempsEcoule.tv_sec = tempsNow.tv_sec - tempsStart.tv_sec - 1;
        tempsEcoule.tv_nsec = tempsNow.tv_nsec - tempsStart.tv_nsec + 1000000000UL;
    } else {
        tempsEcoule.tv_sec = tempsNow.tv_sec - tempsStart.tv_sec;
        tempsEcoule.tv_nsec = tempsNow.tv_nsec - tempsStart.tv_nsec;
    }

    if (tempsEcoule.tv_sec >= TEMPS_PARCOURS) {
        return -1;
    }

    updateTimeDisplay();

    return (1000000000UL - tempsEcoule.tv_nsec) / 1000000UL;
}

void stopParcours()
{
    pthread_cancel(tParcours);
    stop();

    updateTimeDisplay();
    printRightLCD(LCD_LINE_1, "FIN");
    showPoints();
    debugSetActive(false);
}

#define UP_TIME 1000
#define HIGH_TIME 3000
#define DOWN_TIME 1000
#define LOW_TIME 2000
#define MAX_VIT MOT_MAX_V

void* TaskParcours(void* pdata)
{
    (void)pdata;

    for (;;) {
        addPoints(1);
        for (int i = 0; i < UP_TIME; i++) {
            float p = (float)i / (float)UP_TIME;
            changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V);
            delay(1);
        }
        addPoints(1);
        changerMoteurs(MOT_MAX_V, MOT_MAX_V);
        delay(HIGH_TIME);

        addPoints(1);
        for (int i = 0; i < DOWN_TIME; i++) {
            float p = (float)i / (float)DOWN_TIME;
            p = 1 - p;
            changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V);
            delay(1);
        }
        addPoints(1);
        changerMoteurs(0, 0);
        delay(LOW_TIME);
    }

    return NULL;
}

