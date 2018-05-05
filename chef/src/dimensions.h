#ifndef __DIMENSIONS_H__
#define __DIMENSIONS_H__

#include <math.h>

// Dimensions pistes
#define M_PISTE_WIDTH 3000
#define M_PISTE_HEIGHT 2000
#define M_PISTE_ORIG_X 0
#define M_PISTE_ORIG_Y 0

// Dimensions robot
#define WIDTH 250 // mm (from meca)
#define HEIGHT 100 // mm (from random);
#define DISTANCE_BETWEEN_WHEELS WIDTH // mm (from meca)
#define WHEEL_DIAMETER 80 // mm (from meca)
#define WHEEL_PERIMETER WHEEL_DIAMETER * M_PI // mm
#define MOTOR_SPEED_GAIN_RPMP_V 233 // rpm/V (from datasheet)
#define MOTOR_SPEED_GAIN MOTOR_SPEED_GAIN_RPMP_V / 60 // rev/s/V
#define MOTOR_NOMINAL_TENSION 24 // V (from datasheet)
#define CODER_RESOLUTION 100 // cycles/rev
#define CODER_DATA_FACTOR 4 // increments/cycles
#define CODER_DATA_RESOLUTION CODER_RESOLUTION * CODER_DATA_FACTOR // cycles/rev
#define CRAN_REDUC_OUT 48 // nb crans (from meca)
#define CRAN_REDUC_IN 12 // nb crans (from meca)
#define REDUC_RATIO CRAN_REDUC_IN / CRAN_REDUC_OUT // reduction ratio

// Constantes asservissement
#define D_DIR_ECART_MIN 1 // mm
#define D_DIR_ECART_MAX 5 // mm
#define O_DIR_ECART_MIN 1 / 360 * 2 * M_PI // rad
#define O_DIR_ECART_MAX 3 / 360 * 2 * M_PI // rad
#define P 2
#define I 0
#define D 0

#endif
