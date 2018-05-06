#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <pthread.h>
#include <time.h>
#include <stdbool.h>

// Constantes
#define DEBUG_INTERVAL_IDLE 1000
#define DEBUG_INTERVAL_ACTIVE 50

// Structures
enum debugArgTypes {
    d,
    f,
    ld,
    lf,
    s
};

struct debugArg {
    enum debugArgTypes type;
    void* var;
    struct debugArg* next;
};

// Public
void configureDebug(); // Avant tous les configure
void registerDebugVar(char* name, enum debugArgTypes type, void* var);
void startDebug(); // Après tous les configure
void deconfigureDebug();
void debugSetActive(bool active);

// Private
void* TaskDebug(void* pdata);

#endif
