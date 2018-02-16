#include "AC.h"

void registerRxHandlerAC(unsigned char code, rxHandler handler)
{
    rxHandlersAC[code] = handler;
}

ISR(USART0_UDRE_vect)
{
    // When a transmit is ready to be done again
    UDR0 = *toSendAC;
    toSendAC++;
    toSendSizeAC--;
    if (toSendSizeAC <= 0) {
        UCSR0B &= ~(1 << UDRIE0);
        TaskHandle_t holder = xSemaphoreGetMutexHolder(sSendAC);
        if (holder != NULL) {
            vTaskNotifyGiveFromISR(holder, NULL);
        }
    }
}

void sendAC(unsigned char code, void* data, size_t size)
{
    xSemaphoreTake(sSendAC, portMAX_DELAY);
    toSendAC = &code;
    toSendSizeAC = sizeof(code);
    UCSR0B |= (1 << UDRIE0);
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    if (size > 0) {
        toSendAC = data;
        toSendSizeAC = size;
        UCSR0B |= (1 << UDRIE0);
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
    xSemaphoreGive(sSendAC);
}

ISR(USART0_RX_vect)
{
    // When a character is received
    vTaskNotifyGiveFromISR(tReaderAC, NULL);
}

unsigned char readByteAC()
{
    while (!bit_is_set(UCSR0A, RXC0)) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
    unsigned char c = UDR0;
    return c;
}

void readAC(void* data, size_t size)
{
    unsigned char* p = data;
    for (int i = 0; i < size; i++) {
        *p = readByteAC();
        p++;
    }
}

void TaskReaderAC(void* pvParameters)
{
    (void)pvParameters;
    unsigned char code;
    for (;;) {
        code = readByteAC();

        rxHandler handler = rxHandlersAC[code];
        if (handler != NULL) {
            handler();
        } else {
            struct A2CD_ERRs err = { ERR_UNKNOWN_CODE };
            sendAC(A2CD_ERR, &err, sizeof(err));
        }
    }
}

void onC2AD_PING()
{
    sendAC(C2AD_PING, NULL, 0);
}

void configureAC()
{
    /* Set baud rate */
    UBRR0 = AC_PRESCALER;

    /* Set off UART baud doubler */
    UCSR0A &= ~(1 << U2X0);

    /* Enable transmitter & receiver with interrupts */
    UCSR0B = (1 << RXCIE0 | 1 << TXEN0 | 1 << RXEN0);

    /* Set 8 bits character and 1 stop bit */
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

    for (int i = 0; i < 256; i++) {
        rxHandlersAC[i] = NULL;
    }
    sSendAC = xSemaphoreCreateMutex();
    xTaskCreate(TaskReaderAC, "TaskReaderAC", 128, NULL, 2, &tReaderAC);

    registerRxHandlerAC(C2AD_PING, onC2AD_PING);
}
