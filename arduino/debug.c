#include <string.h>
#include "debug.h"
#include "AC.h"

#include "position.h"
#include "movement.h"

void TaskDebug(void *pvParameters) {
    (void) pvParameters;
    for (;;) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        // Copie des valeurs à envoyer en debug
        memcpy((void*) &debug.actuel, (const void*) &actuel, (unsigned long) sizeof(actuel));
        memcpy((void*) &debug.destination, (const void*) &destination, (unsigned long) sizeof(destination));
        debug.movement = movement;

        // Envoi des valeurs
        sendAC(A2CI_DBG, &debug, sizeof(debug));
    }
}

void onA2CI_DBG() {
    vTaskNotifyGiveFromISR(tDebug, NULL);
}


void configureDebug() {
    registerRxHandlerAC(A2CI_DBG, onA2CI_DBG);
    xTaskCreate(TaskDebug, "Debug", 128, NULL, 10, &tDebug);;
}

