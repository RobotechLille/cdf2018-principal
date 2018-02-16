#include "position.h"
#include "AC.h"

void TaskPosition(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = 100 / portTICK_PERIOD_MS;

    ulTaskNotifyTake(pdFALSE, portMAX_DELAY); // TODO Dummy
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



void configurePosition() {
    actuel.x = 0;
    actuel.y = 0;
    actuel.o = 90;

    xTaskCreate(TaskPosition, "Position", 128, NULL, 2, &tPosition);;
}

