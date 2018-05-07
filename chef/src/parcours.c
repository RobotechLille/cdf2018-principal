#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "debug.h"
#include "lcd.h"
#include "movement.h"
#include "motor.h"
#include "parcours.h"
#include "points.h"
#include "position.h"

pthread_t tParcours;
bool isOrange;
struct timespec tempsStart;
struct timespec tempsNow;
struct timespec tempsEcoule = { 0, 0 };

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
    enableConsigne();
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
    disableConsigne();

    updateTimeDisplay();
    printRightLCD(LCD_LINE_1, "FIN");
    showPoints();
    debugSetActive(false);
}

void* TaskParcours(void* pdata)
{
    (void)pdata;

    struct position dest1 = {0, 0, 0};
    setDestination(&dest1);

    sleep(1);

    struct position dest2 = {0, 0, M_PI_2};
    setDestination(&dest2);

    sleep(10);

    stop();

    /*  */
    /* struct position dest3 = {1000, 1000, 0}; */
    /* setDestination(&dest3); */
    /*  */
    /* sleep(5); */



    return NULL;
}
