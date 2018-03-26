#include "position.h"
#include "AC.h"
#include "AF.h"
#include "dimensions.h"

void TaskPosition(void* pvParameters)
{
    (void)pvParameters;

    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for (;;) {
        sendAF(F2AI_CODER, NULL, 0);

        ulTaskNotifyTake(pdFALSE, portMAX_DELAY); // Wait until new information has arrived

        nbCalcPos++;

        float adjacent = DISTANCE_BETWEEN_WHEELS;
        float opposite = deltaCoder.dR - deltaCoder.dL;
        float deltaO = atan(opposite / adjacent);

        float deltaD = (deltaCoder.dL + deltaCoder.dR) / 2;

        actuel.o += deltaO;
        float deltaX = deltaD * cos(actuel.o);
        float deltaY = deltaD * sin(actuel.o);
        actuel.x += deltaX;
        actuel.y += deltaY;

        vTaskDelay(xDelay);
    }
}

void onF2AI_CODER()
{
    readAF(&deltaCoder, sizeof(struct F2AI_CODERs));
    vTaskNotifyGiveFromISR(tPosition, NULL);
}

void configurePosition()
{
    actuel.x = 0;
    actuel.y = 0;
    actuel.o = 0;

    registerRxHandlerAF(F2AI_CODER, onF2AI_CODER);

    xTaskCreate(TaskPosition, "Position", 128, NULL, 2, &tPosition);
    ;
}
