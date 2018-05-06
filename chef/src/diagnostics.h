#ifndef __DIAGNOSTICS_H_
#define __DIAGNOSTICS_H_

#include <stdbool.h>

// Constantes
#define DIAGNOSTIC_INTERVAL 100
#define DIAGNOSTIC_POLL_INTERVAL 100
#define DIAGNOSTIC_SERIAL_TIMEOUT 10000

#define DIAGNOSTIC_TENSION_TEST 3
#define DIAGNOSTIC_CODEUSES_DIFF_MIN 1000
#define DIAGNOSTIC_TEMPS_ROTATION 250

// Public
void runDiagnostics();

// Private
void execDiagnostic(char *name, bool (*diagnostic)(void* arg), void* arg);

#endif
