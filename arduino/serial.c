#include <avr/interrupt.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <queue.h>

#include "serial.h"
#include "ACsignals.h"

ISR(USART0_UDRE_vect) {
    // When a transmit is ready to be done again
    TaskHandle_t holder = xSemaphoreGetMutexHolder(sSendAC);
    if (holder != NULL) {
        vTaskResume(holder);
    }
}

void sendByteAC(unsigned char data) {
    while (!bit_is_set(UCSR0A, UDRE0)) {
        vTaskSuspend(xSemaphoreGetMutexHolder(sSendAC));
    }
    UDR0 = data;
}

void sendAC(unsigned char code, void* data, size_t size) {
    xSemaphoreTake(sSendAC, 0);
    sendByteAC(code);
    unsigned char* p = data;
    for (int i = 0; i < size; i++) {
        sendByteAC(*p++);
    }
    xSemaphoreGive(sSendAC);
}

ISR(USART0_RX_vect) {
    // When a character is received
    vTaskResume(tReaderAC);
}


unsigned char readAC() {
    while (!bit_is_set(UCSR0A, RXC0)) {
        vTaskSuspend(tReaderAC);
    }
    return UDR0;
}

void TaskReaderAC(void *pvParameters) {
    (void) pvParameters;
    unsigned char code;
    for (;;) {
        code = readAC();

        char* sending = "Bonjour ! Comment va ?";
        sendAC('@', sending, 22);
    }
}

void configureAC() {
    /* Set baud rate */
    UBRR0 = AC_PRESCALER;

    /* Set off UART baud doubler */
    UCSR0A &= ~(1 << U2X0);

    /* Enable transmitter & receiver with interrupts */
    UCSR0B = (1 << RXCIE0 | 1 << UDRIE0 | 1 << TXEN0 | 1 << RXEN0);

    /* Set 8 bits character and 1 stop bit */
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

    sSendAC = xSemaphoreCreateMutex();
    xTaskCreate(TaskReaderAC, "TaskReaderAC", 128, NULL, 2, &tReaderAC);
}

