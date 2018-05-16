#ifndef __CALIBRAGE_H_
#define __CALIBRAGE_H_

#include <stdbool.h>

// Constantes

#define CALIBRAGE_TEMPS_ACQUISITION 30000
#define CALIBRAGE_DISTANCE_X 300
#define CALIBRAGE_DISTANCE_Y 300
#define CALIBRAGE_MOVAVG_SIZE 4

// Public
void calibrer(bool orange);

#endif
