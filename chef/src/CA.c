#include "CA.h"
#include <fcntl.h>   // O_*
#include <stdio.h>   // printf...
#include <stdlib.h>  // stuff
#include <string.h>  // memcpy
#include <strings.h> // bzero
#include <unistd.h>  // read(), write()...

void printData(void* data, size_t size)
{
    printf(" ");
    unsigned char* p = data;
    for (size_t i = 0; i < size; i++) {
        if (*p >= ' ' && *p <= '~') {
            printf("  %c", *p);
        } else {
            printf(" %02x", *p);
        }
        p++;
    }
    printf("\n");
}

void configureArduino()
{
    // Connection au port série
    printf("Connexion à %s... ", ARDUINO_PORTNAME);
    arduino = open(ARDUINO_PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY);
    if (arduino < 0) {
        printf("Échec !\n");
        exit(1);
    }

    // Configuration du port série
    struct termios cfg;
    bzero(&cfg, sizeof(cfg));
    cfg.c_cflag = CLOCAL | CREAD | CA_BAUDRATE | CS8;
    cfg.c_iflag = 0;
    cfg.c_oflag = 0;
    cfg.c_lflag = 0;     /* set input mode (non-canonical, no echo,...) */
    cfg.c_cc[VTIME] = 0; /* inter-character timer unused */
    cfg.c_cc[VMIN] = 1;  /* blocking read until 1 char received */
    if (tcsetattr(arduino, TCSANOW, &cfg) < 0) {
        perror("serialConfig.tcsetattr");
        exit(1);
    }

    sleep(1);

    // Flush
    unsigned char trash[1024];
    read(arduino, &trash, sizeof(trash));

    printf("OK!\n");
}

void deconfigureArduino()
{
    close(arduino);
    printf("Déconnecté\n");
}

void registerRxHandler(unsigned char code, rxHandler handler)
{
    rxHandlersAC[code] = handler;
}

void* TaskReaderAC(void* pdata)
{
    (void)pdata;
    unsigned char code;
    for (;;) {
        code = readByteCA();

#ifdef PRINTRAWDATA
        printf("↓");
        printData(&code, sizeof(code));
#endif
        rxHandler handler = rxHandlersAC[code];
        if (handler != NULL) {
            handler();
        } else {
            printf("Code inconnu: %x (%c)\n", code, code);
        }
    }
    return NULL;
}

void onA2CD_ERR()
{
    struct A2CD_ERRs s;
    readCA(&s, sizeof(struct A2CD_ERRs));
    printf("Erreur reçue : %c (%2x)\n", s.code, s.code);
}

void configureCA()
{
    configureArduino();
    for (int i = 0; i < 256; i++) {
        rxHandlersAC[i] = NULL;
    }

    pthread_mutex_init(&sSendCA, NULL);
    pthread_create(&tReaderAC, NULL, TaskReaderAC, NULL);
    registerRxHandler(A2CD_ERR, onA2CD_ERR);
}

void deconfigureCA()
{
    deconfigureArduino();
}

void sendByteCA(unsigned char data)
{
    write(arduino, &data, sizeof(data));

#ifdef PRINTRAWDATA
    printf("↑");
    printData(&data, sizeof(data));
#endif
}

void sendCA(unsigned char code, void* data, size_t size)
{
    pthread_mutex_lock(&sSendCA);

    sendByteCA(code);
    if (size > 0) {
        unsigned char* p = data;
        for (size_t i = 0; i < size; i++) {
            write(arduino, p, sizeof(unsigned char));
            p++;
        }
        // Envoyer plus d'un octet d'un coup curieusement il aime pas ça du tout
    }
    pthread_mutex_unlock(&sSendCA);

#ifdef PRINTRAWDATA
    if (size > 0) {
        printf("↑");
        printData(data, size);
    }
#endif
}

unsigned char readByteCA()
{
    unsigned char c;

    while (read(arduino, &c, sizeof(c)) < 1) {
        sleep(0);
    }
    return c;

#ifdef PRINTRAWDATA
    printf("↓");
    printData(&c, sizeof(c));
#endif
}

void readCA(void* data, size_t size)
{
    size_t remaining = size;
    int justRead;
    char* p = data;
    do {
        justRead = read(arduino, p, remaining);
        if (justRead > 0) {
            p += justRead;
            remaining -= justRead;
        }
    } while (remaining > 0);

#ifdef PRINTRAWDATA
    printf("↓");
    printData(data, size);
#endif
}
