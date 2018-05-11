/*
 * Fonctions de calcul de la position du robot
 */

#ifndef __POSITION_H_
#define __POSITION_H_

// #define INVERSE_L_CODER
#define INVERSE_R_CODER

// Structures
struct __attribute__ ((packed)) position {
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

#endif

