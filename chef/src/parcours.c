#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "actionneurs.h"
#include "debug.h"
#include "lcd.h"
#include "motor.h"
#include "parcours.h"
#include "points.h"
#include "securite.h"
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

    resetActionneurs();
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
    disableAsservissement();
    stop();

    resetLCD();
    updateTimeDisplay();
    printRightLCD(LCD_LINE_1, "FIN");
    showPoints();
    debugSetActive(false);
}

void gotoPoint(float x, float y, float o)
{
    /* if (isOrange) { */
    /*     x = M_PISTE_WIDTH - x; */
    /*     if (!isnan(o)) { */
    /*         o = M_PI - o; */
    /*     } */
    /* } */
    if (isOrange) {
        o = -o;
        y = -y;
    }
    struct position pos = { x, y, o };
    setDestination(&pos);
    printf("New dest : %f %f %f\n", pos.x, pos.y, pos.o);
    waitDestination();
    printf("Done.\n");
    brake();
}

void recuperBalles()
{
    setLoquet(false);
    for (int i = 0; i < NB_BALLES; i++) {
        barilletSuivant();
    }
    setLoquet(true);
}

void* TaskParcours(void* pdata)
{
    (void)pdata;

    float x = 306 + (isOrange ? 170 : 0);

    setSecurite(true, false);
    gotoPoint(x, 200, -M_PI_2);
    setSecurite(false, true);
    gotoPoint(x, 20, -M_PI_2);
    brake();
    for (int i = 0; i < 3; i++) {
        setLoquet(true);
        setLoquet(false);
    }
    addPoints(10);
    gotoPoint(x, 200, ANGLE_INSIGNIFIANT);
    setSecurite(true, false);
    gotoPoint(600, 50, ANGLE_INSIGNIFIANT);
    disableAsservissement();
    stop();
    return NULL;
}

void* TaskParcoursVrai(void* pdata)
{
    (void)pdata;

    // Récupération des balles
    gotoPoint(X_RECUP_1, Y_RECUP_1, O_RECUP_1);
    setLoquet(false);
    recuperBalles();

    // Lancement des balles
    gotoPoint(X_LANCER, Y_LANCER, O_LANCER);
    setPositionBalle(ejection);
    setPropulsion(true);
    pousserBalle();
    for (int i = 0; i < NB_BALLES - 1; i++) {
        barilletSuivant();
        pousserBalle();
    }
    setPropulsion(false);
    setPositionBalle(attente);

    // Évitement des cubes
    gotoPoint(X_EVIT, Y_EVIT, O_EVIT);

    // Aller à l'abeille
    gotoPoint(X_ABEILLE, Y_ABEILLE, O_ABEILLE);

    // Récupération des balles
    gotoPoint(X_RECUP_2, Y_RECUP_2, O_RECUP_2);
    recuperBalles();

    // Dépot des balles adverses
    gotoPoint(X_USE, Y_USE, O_USE);
    setPositionBalle(evacuation);
    barilletSuivant(); // TODO Peut-être pas utile en fonction
    // de quelle balle arrive en premier
    pousserBalle();
    for (int i = 0; i < NB_BALLES / 2 - 1; i++) {
        barilletSkip();
        pousserBalle();
    }
    setPositionBalle(attente);

    // Lancement des balles
    gotoPoint(X_LANCER, Y_LANCER, O_LANCER);
    setPositionBalle(ejection);
    setPropulsion(true);
    barilletSuivant();
    pousserBalle();
    for (int i = 0; i < NB_BALLES / 2 - 1; i++) {
        barilletSkip();
        pousserBalle();
    }
    setPropulsion(false);
    setPositionBalle(attente);

    stop();

    return NULL;
}

