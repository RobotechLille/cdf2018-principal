#ifndef __DIAGNOSTICS_H_
#define __DIAGNOSTICS_H_

#include <stdbool.h>

// Constantes
#define DIAGNOSTIC_INTERVAL 500
#define DIAGNOSTIC_POLL_INTERVAL 100
#define DIAGNOSTIC_SERIAL_TIMEOUT 10000

// Public
void runDiagnostics();

// Private
void execDiagnostic(char *name, bool (*diagnostic)(void));

#endif
