#include <avr/io.h>
#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <task.h>

#include "AC.h"
#include "AF.h"
#include "position.h"
#include "movement.h"
#include "debug.h"

void TaskBlink(void *pvParameters) {
    (void) pvParameters;
    TickType_t xLastWakeTime;
    TickType_t xFrequency = 200 / portTICK_PERIOD_MS;

    DDRB = 0xFF;

    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        PORTB = PINB ^ 0xFF;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

int main(void) {
    configureAC(); // Doit rester en premier :)
    configureAF(); // Doit rester en premier :)
    configureMovement();
    configurePosition();
    configureDebug();

    xTaskCreate(TaskBlink, "Blink", 128, NULL, 2, NULL);
    sei();

    vTaskStartScheduler();
    return 0;
}
