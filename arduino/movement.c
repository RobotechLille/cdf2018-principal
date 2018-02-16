#include "movement.h"
#include "AC.h"
#include "position.h"

void TaskMovement(void* pvParameters)
{
    (void)pvParameters;
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

        if (true) {                    // Arrivé à destination
            sendAC(movement, NULL, 0); // On rapporte au chef qu'on a terminé l'action en cours
            brake();

            ulTaskNotifyTake(pdFALSE, portMAX_DELAY); // Mettre en veille jusqu'à l'arrivée de la prochaine instruction
            xLastWakeTime = xTaskGetTickCount();
        } else {
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
}

void brake()
{
    movement = C2AD_BRAKE;
    // TODO Mettre les IN à ce qu'il faut
}

void onC2AD_BRAKE()
{
    brake();
    vTaskNotifyGiveFromISR(tMovement, NULL);
}

void freewheel()
{
    movement = C2AD_FREE;
    // TODO Mettre les IN à ce qu'il faut
}

void onC2AD_FREE()
{
    freewheel();
}

void onC2AD_GOTO()
{
    movement = C2AD_GOTO;
    readAC(&destination, sizeof(struct C2AD_GOTOs));
    vTaskNotifyGiveFromISR(tMovement, NULL);
}

void stop()
{
    brake();
    // TODO Actionneurs
}

void onC2AD_STOP()
{
    stop();
    sendAC(C2AD_STOP, NULL, 0);
}

void configureMovement()
{
    // TODO Configuration des pins

    freewheel();

    registerRxHandlerAC(C2AD_BRAKE, onC2AD_BRAKE);
    registerRxHandlerAC(C2AD_STOP, onC2AD_STOP);
    registerRxHandlerAC(C2AD_FREE, onC2AD_FREE);
    registerRxHandlerAC(C2AD_GOTO, onC2AD_GOTO);

    xTaskCreate(TaskMovement, "Movement", 128, NULL, 2, &tMovement);
}
