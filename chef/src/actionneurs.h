// Robotech Lille 2017-2018

#ifndef __ACTIONNEURS_H_
#define __ACTIONNEURS_H_

#include <stdbool.h>

enum positionBalle {
    attente,
    evacuation,
    ejection
};

// Public

// Specific

// True: Ouvert, False: fermé
void setLoquet(bool state);
void barilletReset();
void barilletSuivant();
void barilletSkip();
void pousserBalle();
void setPositionBalle(enum positionBalle pos);
void setPropulsion(bool state);

// Common

void configureActionneurs();
void resetActionneurs();
void stopActionneurs();
void deconfigureActionneurs();

// Private

void attendAction(unsigned char code);

#endif
