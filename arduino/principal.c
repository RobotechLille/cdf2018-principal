#include <avr/io.h>
#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <task.h>

#include "serial.h"

unsigned char speed = 200;

void TaskBlink(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = speed / portTICK_PERIOD_MS;

    DDRB = 0xFF;

    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        PORTB = PINB ^ 0xFF;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

int main(void) {
    configureAC();
    sei();

    xTaskCreate(TaskBlink, "Blink", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
