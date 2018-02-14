#include "position.h"
#include "AC.h"

void TaskPosition(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = 100 / portTICK_PERIOD_MS;

    vTaskSuspend(tPosition); // TODO Dummy
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}



void configurePosition() {
    xTaskCreate(TaskPosition, "Position", 128, NULL, 2, &tPosition);;
}

