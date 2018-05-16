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
#define MOTOR_CONTROLLER_REFERENCE 5.0 // V (from wiring)
#define MOTOR_SATURATION_MIN 0.0 // V (from random)
#define MOTOR_SATURATION_MAX 4.0 // V (from testing)
#define PWM_MAX 3.3 // V (from FPGA datasheet)
#define CODER_RESOLUTION 370.0 // cycles/motor rev
#define CODER_DATA_FACTOR 4.0 // increments/motor cycles
#define CODER_DATA_RESOLUTION (CODER_RESOLUTION * CODER_DATA_FACTOR) // cycles/motor rev
#define CRAN_REDUC_OUT 48.0 // nb crans (from meca)
#define CRAN_REDUC_IN 12.0 // nb crans (from meca)
#define REDUC_RATIO (CRAN_REDUC_IN / CRAN_REDUC_OUT) // reduction ratio
#define CODER_FULL_RESOLUTION (CODER_DATA_RESOLUTION / REDUC_RATIO) // cycles / wheel rev
#define AV_PER_CYCLE (WHEEL_PERIMETER / CODER_FULL_RESOLUTION) // mm

// Pour éviter les pics de codeuse liées à la communication I2C
#define ABSOLUTE_MAX_VITESSE_ROBOT 10.0 // km/h
#define ABSOLUTE_MAX_VITESSE_ROBOT_MMP_S (ABSOLUTE_MAX_VITESSE_ROBOT * 10000.0 / 36.0) // mm/s
#define ABSOLUTE_MAX_VITESSE_ROBOT_REVP_S (ABSOLUTE_MAX_VITESSE_ROBOT_MMP_S / WHEEL_PERIMETER) // rev/s
#define ABSOLUTE_MAX_VITESSE_ROBOT_CYCP_S (ABSOLUTE_MAX_VITESSE_ROBOT_REVP_S * CODER_FULL_RESOLUTION) // cycle/s

// Constantes asservissement

// Asservissement en angle
#define O_VIT_MIN 0.5 // rad/s
#define O_TENSION_MIN 1 // V
#define O_DIR_ECART_MIN (20.0 / 360.0 * 2.0 * M_PI) // rad
#define O_ECART_MIN (10.0 / 360.0 * 2.0 * M_PI) // rad
#define O_ECART_MAX (20.0 / 360.0 * 2.0 * M_PI) // rad
#define DERIV_M_PI (MOTOR_SATURATION_MAX / (WHEEL_PERIMETER * M_PI))
#define O_KP (3.0 * DERIV_M_PI) // au max peut dérivier de pi
#define O_KI 0.0
#define O_KD 0.0
#define CAROTTE_ANGLE (TARGET_TENSION / O_KP) // mm

// Asservissement en distance
#define D_VIT_MIN 10.0 // mm/s
#define D_TENSION_MIN 1 // V
#define D_DIR_ECART_MIN 20.0 // mm
#define D_DIR_ECART_MAX 70.0 // mm
#define D_KP 0.1
#define D_KI 0.0
#define D_KD 0.0
#define TARGET_TENSION_RATIO 0.75
#define TARGET_TENSION (TARGET_TENSION_RATIO * MOTOR_SATURATION_MAX) // V
#define CAROTTE_DISTANCE (TARGET_TENSION / D_KP) // mm

#define MARGE_SECURITE 300.0 // mm

#endif
