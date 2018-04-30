#ifndef __PARCOURS_H__
#define __PARCOURS_H__

#include <stdbool.h>

#define TEMPS_PARCOURS 100

void prepareParcours(bool orange);
void startParcours();
// Returns : -1 if parcours ended, N ms for the next time it should be checked
int updateParcours();
void stopParcours();
void* TaskParcours(void* pdata);

#endif
