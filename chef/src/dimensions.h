#ifndef __DIMENSIONS_H__
#define __DIMENSIONS_H__

#include <math.h>

// TODO None of that is verified

// [mm]
#define WHEEL_DIAMETER 80.0
// [mm]
#define WHEEL_PERIMETER WHEEL_DIAMETER * M_PI
// [cycles/revolution]
#define CODER_RESOLUTION 100
// [increments/revolution]
#define CODER_DATA_RESOLUTION CODER_RESOLUTION * 4
// [mm]
#define CODER_DELTA_DISTANCE WHEEL_PERIMETER / CODER_DATA_RESOLUTION
// [mm]
#define DISTANCE_BETWEEN_WHEELS 250.0

#endif
