/*
 * Fonctions de calcul de la position du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

// #define INVERSE_L_CODER
#define INVERSE_R_CODER

#define POSITION_INTERVAL 10

#define VIT_MOVAVG_TIME 100
#define VIT_MOVAVG_SIZE (VIT_MOVAVG_TIME / POSITION_INTERVAL)

// Structures
struct __attribute__((packed)) position {
    float x;
    float y;
    float o;
};

// Fonctions
void configurePosition();
void deconfigurePosition();
void getCoders(long* l, long* r);
unsigned int getPositionNewer(struct position* pos, unsigned int lastCalc);
unsigned int getPosition(struct position* pos);
void setPosition(struct position* pos);
void resetPosition();
float getAnglVitesse();
float getAbsVitesse();

#endif
