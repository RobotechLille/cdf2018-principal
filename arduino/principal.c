#include <FreeRTOS.h>
#include <task.h>
#include <avr/io.h>

void TaskBlink(void *pvParameters)
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 200 / portTICK_PERIOD_MS;

    DDRB = 0xFF;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        PORTB = PINB ^ 0xFF;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

int main(void)
{
    xTaskCreate(TaskBlink, "Blink", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
