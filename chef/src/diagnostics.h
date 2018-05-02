#ifndef __DIAGNOSTICS_H_
#define __DIAGNOSTICS_H_

#include <stdbool.h>

// Constantes
#define DIAGNOSTIC_INTERVAL 500
#define DIAGNOSTIC_POLL_INTERVAL 100
#define DIAGNOSTIC_SERIAL_TIMEOUT 10000

#define DIAGNOSTIC_TENSION_TEST 1
#define DIAGNOSTIC_CODEUSES_DIFF_MIN 1000

// Public
void runDiagnostics();

// Private
void execDiagnostic(char *name, bool (*diagnostic)(void* arg), void* arg);

#endif
