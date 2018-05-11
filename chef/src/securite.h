#ifndef __SECURITE_H_
#define __SECURITE_H_

#include <pthread.h>

#define SOUND_MM_P_MS 0.3312
#define SENSOR_SAMPLING_INTERVAL 60

// Fonctions
void configureSecurite();
void deconfigureSecurite();
void getDistance(float* avant, float* arriere);
void getAllDistance(float* frontL, float* frontR, float* backL, float* backR);

#endif

