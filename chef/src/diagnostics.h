#ifndef __DIAGNOSTICS_H_
#define __DIAGNOSTICS_H_

#include <stdbool.h>

// Constantes
#define DIAGNOSTIC_INTERVAL 100
#define DIAGNOSTIC_POLL_INTERVAL 100
#define DIAGNOSTIC_SERIAL_TIMEOUT 10000

#define DIAGNOSTIC_TENSION_TEST 3
#define DIAGNOSTIC_CODEUSES_DIFF_MIN 100
#define DIAGNOSTIC_TEMPS_ROTATION 500

typedef bool (*diagnosticFunc)(void* arg);

// Public
void runDiagnostics();

// Private
void execDiagnostic(char* name, diagnosticFunc diag, void* arg);

#endif
