#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#include "parcours.h"
#include "movement.h"
#include "position.h"
#include "points.h"
#include "lcd.h"

pthread_t tParcours;
bool isOrange;
char tempLine[16];
struct timespec tempsStart;
struct timespec tempsNow;
struct timespec tempsEcoule;

void prepareParcours(bool orange)
{
    isOrange = orange;
    clearLCD();
    gotoLCD(LCD_LINE_1);
    sprintf(tempLine, "--:--/%2d:%02d", TEMPS_PARCOURS / 60, TEMPS_PARCOURS % 60);
    printLCD(tempLine);
    gotoLCD(LCD_LINE_1 + 16 - 3);
    printLCD("ATT");

    resetPoints();
    showPoints();
    gotoLCD(LCD_LINE_2 + 16 - 3);
    printLCD(isOrange ? "Org" : "Vrt");
}

void startParcours()
{
    clock_gettime(CLOCK_REALTIME, &tempsStart);
    pthread_create(&tParcours, NULL, TaskParcours, NULL); // TODO Start on mutex unlock
    lockLCD();
    gotoLCD(LCD_LINE_1 + 16 - 3);
    printLCD("   ");
    unlockLCD();
}

void updateTimeDisplay()
{
    lockLCD();
    gotoLCD(LCD_LINE_1);
    sprintf(tempLine, "%2ld:%02ld", tempsEcoule.tv_sec / 60, tempsEcoule.tv_sec % 60);
    printLCD(tempLine);
    unlockLCD();
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
    gotoLCD(LCD_LINE_1 + 16 - 3);
    printLCD("FIN");
    showPoints();
}

void* TaskParcours(void* pdata)
{
    (void)pdata;

    for (;;) {
        delay(250);
        addPoints(1);
    }
}

void* TaskParcours2(void* pdata)
{
    (void)pdata;

    /* struct position pos; */
    /* for (;;) { */
    /*     pos.x = (int) (rand()*200.0/RAND_MAX); */
    /*     pos.y = (int) (rand()*100.0/RAND_MAX); */
    /*     pos.o = (int) (rand()*360.0/RAND_MAX); */
    /*     aller(&pos); */
    /*     sleep(1); */
    /*     brake(); */
    /*     sleep(2); */
    /* } */

    struct timespec tim; // 10 ms
    tim.tv_sec = 0;
    tim.tv_nsec = 10000000L;

#define RAMP_TIME 100
#define MAX_VIT MOT_MAX_V

    /* for (;;) { */
    // ↗
    for (int i = 0; i < RAMP_TIME; i++) {
        float p = (float)i / (float)RAMP_TIME;
        changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V);
        nanosleep(&tim, NULL);
    }
    changerMoteurs(MOT_MAX_V, MOT_MAX_V);
    // ↑
    sleep(2);
    /*     // ↘ */
    /*     for (int i = 0; i < RAMP_TIME; i++) { */
    /*         float p = (float) i / (float) RAMP_TIME; */
    /*         p = 1 - p; */
    /*         changerMoteurs(p * MOT_MAX_V, p * MOT_MAX_V); */
    /*         nanosleep(&tim, NULL); */
    /*     } */
    /*     sleep(5); */
    /* } */

    return NULL;
}
