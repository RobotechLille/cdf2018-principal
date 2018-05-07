#ifndef __DIMENSIONS_H__
#define __DIMENSIONS_H__

#include <math.h>

// Dimensions pistes
#define M_PISTE_WIDTH 3000.0
#define M_PISTE_HEIGHT 2000.0
#define M_PISTE_ORIG_X 0.0
#define M_PISTE_ORIG_Y 0.0

// Dimensions robot
#define WIDTH 250.0 // mm (from meca)
#define HEIGHT 100.0 // mm (from random)
#define DISTANCE_BETWEEN_WHEELS WIDTH // mm (from meca)
#define WHEEL_DIAMETER 80.0 // mm (from meca)
#define WHEEL_PERIMETER (WHEEL_DIAMETER * M_PI) // mm
#define MOTOR_SPEED_GAIN_RPMP_V 233.0 // rpm/V (from datasheet)
#define MOTOR_SPEED_GAIN (MOTOR_SPEED_GAIN_RPMP_V / 60.0) // motor rev/s/V
#define MOTOR_NOMINAL_TENSION 24.0 // V (from datasheet)
#define MOTOR_CONTROLLER_ALIMENTATION 24.0 // V (from elec)
#define MOTOR_CONTROLLER_REFERENCE 5 // V (from wiring)
#define MOTOR_SATURATION_MIN 0 // V (from random)
#define MOTOR_SATURATION_MAX 12.0 // V (from testing)
#define PWM_MAX 3.3 // V (from FPGA datasheet)
#define CODER_RESOLUTION 370.0 // cycles/motor rev
#define CODER_DATA_FACTOR 4.0 // increments/motor cycles
#define CODER_DATA_RESOLUTION (CODER_RESOLUTION * CODER_DATA_FACTOR) // cycles/motor rev
#define CRAN_REDUC_OUT 48.0 // nb crans (from meca)
#define CRAN_REDUC_IN 12.0 // nb crans (from meca)
#define REDUC_RATIO (CRAN_REDUC_IN / CRAN_REDUC_OUT) // reduction ratio
#define CODER_FULL_RESOLUTION (CODER_DATA_RESOLUTION / REDUC_RATIO) // cycles / wheel rev
#define AV_PER_CYCLE (WHEEL_PERIMETER / CODER_FULL_RESOLUTION) // mm

// Constantes asservissement
#define D_DIR_ECART_MIN 1.0 // mm
#define D_DIR_ECART_MAX 5.0 // mm
#define O_DIR_ECART_MIN (6.0 / 360.0 * 2.0 * M_PI) // rad
#define O_DIR_ECART_MAX (45.0 / 360.0 * 2.0 * M_PI) // rad
#define O_GAIN 3.0
#define P 3.0
#define I 0.0
#define D 0.0

#endif
