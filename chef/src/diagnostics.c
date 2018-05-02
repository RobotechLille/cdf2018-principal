#include <unistd.h>

#include "diagnostics.h"
#include "buttons.h"
#include "lcd.h"

#include "CF.h"
#include "movement.h"

bool recu;

void setRecu()
{
    recu = true;
}

bool diagFPGA(void* arg)
{
    (void) arg;

    recu = false;
    registerRxHandler(C2FD_PING, setRecu);
    sendCF(C2FD_PING, NULL, 0);

    for (int i = 0; i <= DIAGNOSTIC_SERIAL_TIMEOUT; i += DIAGNOSTIC_POLL_INTERVAL) {
        if (recu) {
            break;
        }
        usleep(DIAGNOSTIC_POLL_INTERVAL * 1000);
    }
    return recu;
}

bool diagArduino(void* arg)
{
    (void) arg;
    return false;
}

bool diagCodeuse(void* arg)
{
    int i = *((int*) arg);
    long lCod, rCod;
    getCoders(&lCod, &rCod);
    float tension = DIAGNOSTIC_TENSION_TEST;
    if (i % 2 == 1) { // Arrière
        tension = - tension;
    }
    printf("49 %f\n", tension);
    if (i < 2) {
        changerMoteurs(tension, 0);
    } else {
        changerMoteurs(0, tension);
    }
    usleep(500*1000);
    brake();
    long lCodn, rCodn;
    getCoders(&lCodn, &rCodn);
    long diff;
    printf("60 %ld %ld %ld %ld\n", lCod, lCodn, rCod, rCodn);
    if (i < 2) {
        diff = lCodn - lCod;
    } else {
        diff = rCodn - rCod;
    }
    printf("65 %ld\n", diff);
    if (i % 2 == 0) { // Avant
        return (diff > DIAGNOSTIC_CODEUSES_DIFF_MIN);
    } else { // Arrière
        return (-diff > DIAGNOSTIC_CODEUSES_DIFF_MIN);
    }
}

void execDiagnostic(char *name, bool (*diagnostic)(void* arg), void* arg)
{
    clearLCD();
    printToLCD(LCD_LINE_1, name);
    printToLCD(LCD_LINE_2, "...");
    bool res = diagnostic(arg);
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
    /* execDiagnostic("Lien Arduino", diagArduino); */
    int i;
    i = 0; execDiagnostic("Mot+Cod L AV", diagCodeuse, &i);
    i = 1; execDiagnostic("Mot+Cod L AR", diagCodeuse, &i);
    i = 2; execDiagnostic("Mot+Cod R AV", diagCodeuse, &i);
    i = 3; execDiagnostic("Mot+Cod R AR", diagCodeuse, &i);
}

