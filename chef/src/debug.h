#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <pthread.h>
#include <time.h>


// Structures
enum debugArgTypes {d, f, ld, lf, s};

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

// Private
void* TaskDebug(void *pdata);

#endif
