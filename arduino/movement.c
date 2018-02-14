#include "movement.h"
#include "AC.h"

void TaskMovement(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = 100 / portTICK_PERIOD_MS;

    vTaskSuspend(tMovement); // Mettre en veille jusqu'à l'arrivée de la prochaine instruction
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {

        // TODO Ici ira le code qui changera les valeurs des moteurs
        vTaskDelayUntil(&xLastWakeTime, 1000 / portTICK_PERIOD_MS);

        if (true) { // Arrivé à destination
            sendAC(movement, NULL, 0); // On rapporte au chef qu'on a terminé l'action en cours
            // Mettre en brake

            // Envoi du message de bonne réception
            vTaskSuspend(tMovement); // Mettre en veille jusqu'à l'arrivée de la prochaine instruction
            xLastWakeTime = xTaskGetTickCount();
        } else {
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
}

void onC2AD_STOP() {
    movement = C2AD_STOP;
    vTaskResume(tMovement);
}

void onC2AD_GOTO() {
    movement = C2AD_GOTO;
    readAC(&destination, sizeof(struct C2AD_GOTOs));
    vTaskResume(tMovement);
}


void configureMovement() {
    // TODO Configuration des pins

    movement = C2AD_STOP;

    registerRxHandler(C2AD_STOP, onC2AD_STOP);
    registerRxHandler(C2AD_GOTO, onC2AD_GOTO);

    xTaskCreate(TaskMovement, "Movement", 128, NULL, 2, &tMovement);;
}

