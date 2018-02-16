#include "movement.h"
#include "position.h"
#include "AC.h"

void TaskMovement(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = 100 / portTICK_PERIOD_MS;

    ulTaskNotifyTake(pdFALSE, portMAX_DELAY); // Mettre en veille jusqu'à l'arrivée de la prochaine instruction
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {

        // TODO Ici ira le code qui changera les valeurs des moteurs
        vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_PERIOD_MS);

        if (movement == C2AD_GOTO) {
            actuel.x = destination.x;
            actuel.y = destination.y;
            actuel.o = destination.o;
        }

        if (true) { // Arrivé à destination
            sendAC(movement, NULL, 0); // On rapporte au chef qu'on a terminé l'action en cours
            // TODO Mettre en brake
            movement = C2AD_STOP;

            ulTaskNotifyTake(pdFALSE, portMAX_DELAY); // Mettre en veille jusqu'à l'arrivée de la prochaine instruction
            xLastWakeTime = xTaskGetTickCount();
        } else {
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
}

void onC2AD_STOP() {
    movement = C2AD_STOP;
    vTaskNotifyGiveFromISR(tMovement, NULL);
}

void onC2AD_GOTO() {
    movement = C2AD_GOTO;
    readAC(&destination, sizeof(struct C2AD_GOTOs));
    vTaskNotifyGiveFromISR(tMovement, NULL);
}


void configureMovement() {
    // TODO Configuration des pins

    movement = C2AD_STOP;

    registerRxHandlerAC(C2AD_STOP, onC2AD_STOP);
    registerRxHandlerAC(C2AD_GOTO, onC2AD_GOTO);

    xTaskCreate(TaskMovement, "Movement", 128, NULL, 2, &tMovement);;
}

