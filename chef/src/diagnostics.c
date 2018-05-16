#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "buttons.h"
#include "diagnostics.h"
#include "lcd.h"

#include "CA.h"
#include "fpga.h"
#include "actionneurs.h"
#include "imu.h"
#include "motor.h"
#include "position.h"

bool recu;

void setRecu()
{
    recu = true;
}

bool diagArduino(void* arg)
{
    (void)arg;

    recu = false;
    registerRxHandlerCA(C2AD_PING, setRecu);
    sendCA(C2AD_PING, NULL, 0);

    for (int i = 0; i <= DIAGNOSTIC_SERIAL_TIMEOUT; i += DIAGNOSTIC_POLL_INTERVAL) {
        if (recu) {
            break;
        }
        usleep(DIAGNOSTIC_POLL_INTERVAL * 1000);
    }
    registerRxHandlerCA(C2AD_PING, NULL);
    return recu;
}

bool diagCodeuse(void* arg)
{
    int i = *((int*)arg);
    long lCod, rCod;
    getCoders(&lCod, &rCod);
    float tension = DIAGNOSTIC_TENSION_TEST;
    if (i % 2 == 1) { // Arrière
        tension = -tension;
    }
    if (i < 2) {
        setMoteurTension(tension, 0);
    } else {
        setMoteurTension(0, tension);
    }
    usleep(DIAGNOSTIC_TEMPS_ROTATION * 1000);
    brake();
    long lCodn, rCodn;
    getCoders(&lCodn, &rCodn);
    long diff;
    if (i < 2) {
        diff = lCodn - lCod;
    } else {
        diff = rCodn - rCod;
    }
    if (i % 2 == 0) { // Avant
        return (diff > DIAGNOSTIC_CODEUSES_DIFF_MIN);
    } else { // Arrière
        return (-diff > DIAGNOSTIC_CODEUSES_DIFF_MIN);
    }
}

bool diagFPGA(void* arg)
{
    (void)arg;
    return connectedFPGA();
}

bool diagIMU(void* arg)
{
    (void)arg;
    return connectedIMU();
}

bool diagJustRun(void* arg)
{
    void (*fonction)(void) = arg;
    fonction();
    usleep(300*1000);
    return true;
}

void diagSetLoquetOuvert()
{
    setLoquet(true);
}
void diagSetLoquetFerme()
{
    setLoquet(false);
}
void diagSetPositionBalleAttente()
{
    setPositionBalle(attente);
}
void diagSetPositionBalleEjection()
{
    setPositionBalle(ejection);
}
void diagSetPositionBalleEvacuation()
{
    setPositionBalle(evacuation);
}
void diagSetPropulsionOn()
{
    setPropulsion(true);
}
void diagSetPropulsionOff()
{
    setPropulsion(false);
}

void execDiagnostic(char* name, diagnosticFunc diag, void* arg)
{
    clearLCD();
    printToLCD(LCD_LINE_1, name);
    printToLCD(LCD_LINE_2, "...");
    bool res = diag(arg);
    if (res) {
        printToLCD(LCD_LINE_2, "Ok!");
        usleep(DIAGNOSTIC_INTERVAL * 1000);
    } else {
        printToLCD(LCD_LINE_2, "Echec!");
        pressedButton(BUT_BLOCK);
    }
}

void runDiagnostics()
{
    execDiagnostic("Lien FPGA", diagFPGA, NULL);
    execDiagnostic("Lien Arduino", diagArduino, NULL);
    /* execDiagnostic("Lien IMU", diagIMU, NULL); */
    int i;
    i = 0;
    execDiagnostic("Mot+Cod L AV", diagCodeuse, &i);
    i = 1;
    execDiagnostic("Mot+Cod L AR", diagCodeuse, &i);
    i = 2;
    execDiagnostic("Mot+Cod R AV", diagCodeuse, &i);
    i = 3;
    execDiagnostic("Mot+Cod R AR", diagCodeuse, &i);

    execDiagnostic("Ouverture loquet", diagJustRun, &diagSetLoquetOuvert);
    execDiagnostic("Fermeture loquet", diagJustRun, &diagSetLoquetFerme);
    /* execDiagnostic("Reset barillet", diagJustRun, &barilletReset); */
    /* execDiagnostic("T+1 barillet", diagJustRun, &barilletSuivant); */
    /* execDiagnostic("T+2 barillet", diagJustRun, &barilletSkip); */
    /* execDiagnostic("Pousser balle", diagJustRun, &pousserBalle); */
    /* execDiagnostic("Pos. ejection", diagJustRun, &diagSetPositionBalleEjection); */
    /* execDiagnostic("Pos. evacuation", diagJustRun, &diagSetPositionBalleEvacuation); */
    /* execDiagnostic("Pos. attente", diagJustRun, &diagSetPositionBalleAttente); */
    /* execDiagnostic("Propulsion on", diagJustRun, &diagSetPropulsionOff); */
    /* execDiagnostic("Propulsion off", diagJustRun, &diagSetPropulsionOn); */
}
