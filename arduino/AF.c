#include "AF.h"
#include "AC.h"

void registerRxHandlerAF(unsigned char code, rxHandler handler)
{
    rxHandlersAF[code] = handler;
}

ISR(USART1_UDRE_vect)
{
    // When a transmit is ready to be done again
    UDR1 = *toSendAF;
    toSendAF++;
    toSendSizeAF--;
    if (toSendSizeAF <= 0) {
        UCSR1B &= ~(1 << UDRIE0);
        TaskHandle_t holder = xSemaphoreGetMutexHolder(sSendAF);
        if (holder != NULL) {
            vTaskNotifyGiveFromISR(holder, NULL);
        }
    }
}

void sendAF(unsigned char code, void* data, size_t size)
{
    xSemaphoreTake(sSendAF, portMAX_DELAY);
    toSendAF = &code;
    toSendSizeAF = sizeof(code);
    UCSR1B |= (1 << UDRIE0);
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    if (size > 0) {
        toSendAF = data;
        toSendSizeAF = size;
        UCSR1B |= (1 << UDRIE0);
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
    xSemaphoreGive(sSendAF);
}

ISR(USART1_RX_vect)
{
    // When a character is received
    vTaskNotifyGiveFromISR(tReaderAF, NULL);
}

unsigned char readByteAF()
{
    while (!bit_is_set(UCSR1A, RXC1)) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
    return UDR1;
}

void readAF(void* data, size_t size)
{
    unsigned char* p = data;
    for (int i = 0; i < size; i++) {
        *p = readByteAF();
        p++;
    }
}

void TaskReaderAF(void* pvParameters)
{
    (void)pvParameters;
    unsigned char code;
    for (;;) {
        code = readByteAF();

        rxHandler handler = rxHandlersAF[code];
        if (handler != NULL) {
            handler();
        } else {
            struct A2CD_ERRs err = { ERR_UNKNOWN_CODE_FPGA };
            sendAC(A2CD_ERR, &err, sizeof(err));
        }
    }
}

void configureAF()
{
    /* Set baud rate */
    UBRR1 = AF_PRESCALER;

    /* Set off UART baud doubler */
    UCSR1A &= ~(1 << U2X1);

    /* Enable transmitter & receiver with interrupts */
    UCSR1B = (1 << RXCIE1 | 1 << TXEN1 | 1 << RXEN1);

    /* Set 8 bits character and 1 stop bit */
    UCSR1C = (1 << UCSZ11 | 1 << UCSZ10);

    for (int i = 0; i < 256; i++) {
        rxHandlersAF[i] = NULL;
    }
    sSendAF = xSemaphoreCreateMutex();
    xTaskCreate(TaskReaderAF, "TaskReaderAF", 128, NULL, 2, &tReaderAF);
}
