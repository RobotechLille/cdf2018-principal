#include <unistd.h>

#include "diagnostics.h"
#include "buttons.h"
#include "lcd.h"

#include "CF.h"

bool recu;

void setRecu()
{
    recu = true;
}

bool diagFPGA()
{
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

bool diagArduino()
{
    return false;
}

void execDiagnostic(char *name, bool (*diagnostic)(void))
{
    clearLCD();
    printToLCD(LCD_LINE_1, name);
    printToLCD(LCD_LINE_2, "...");
    bool res = diagnostic();
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
    execDiagnostic("Lien FPGA", diagFPGA);
    /* execDiagnostic("Lien Arduino", diagArduino); */
}

