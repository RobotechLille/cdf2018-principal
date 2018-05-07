#include "CA.h"
#include <fcntl.h>   // O_*
#include <stdio.h>   // printf...
#include <stdlib.h>  // stuff
#include <string.h>  // memcpy
#include <strings.h> // bzero
#include <unistd.h>  // read(), write()...

int arduino;
pthread_mutex_t sSendCA;
pthread_t tReaderAC;

rxHandler rxHandlersAC[256];
bool pret;


void printDataCA(void* data, size_t size)
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
    fflush(stdout);
    arduino = open(ARDUINO_PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if (arduino < 0) {
        printf("Échec !\n");
        exit(1);
    }

    // Configuration du port série
    fcntl(arduino, F_SETFL, O_RDWR);

    struct termios cfg;

    tcgetattr(arduino, &cfg);

    cfmakeraw(&cfg);
    cfsetispeed(&cfg, CA_BAUDRATE);
    cfsetospeed(&cfg, CA_BAUDRATE);

    cfg.c_cflag |= (CLOCAL | CREAD);
    cfg.c_cflag &= ~PARENB;
    cfg.c_cflag &= ~CSTOPB;
    cfg.c_cflag &= ~CSIZE;
    cfg.c_cflag |= CS8;
    cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    cfg.c_oflag &= ~OPOST;

    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 10;

    if (tcsetattr(arduino, TCSANOW, &cfg) < 0) {
        perror("serialConfig.tcsetattr");
        exit(1);
    }

    int status;
    ioctl(arduino, TIOCMGET, &status);

    status |= TIOCM_DTR;
    status |= TIOCM_RTS;

    ioctl(arduino, TIOCMSET, &status);

    usleep(10 * 1000);

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

void registerRxHandlerCA(unsigned char code, rxHandler handler)
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
        printDataCA(&code, sizeof(code));
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

void setPretCA()
{
    pret = true;
}

void doNothingCA()
{
}

void configureCA()
{
    configureArduino();
    for (int i = 0; i < 256; i++) {
        rxHandlersAC[i] = NULL;
    }

    pthread_mutex_init(&sSendCA, NULL);
    pthread_create(&tReaderAC, NULL, TaskReaderAC, NULL);

    printf("Attente de réponse du Arduino... ");
    fflush(stdout);
    // Dans le cas où on aurait laissé l'Arduino en attente de donnée,
    // on envoie des pings en boucle jusqu'à ce qu'il nous réponde.
    pret = false;
    registerRxHandlerCA(C2AD_PING, setPretCA);
    while (!pret) {
        sendCA(C2AD_PING, NULL, 0);
        usleep(100 * 1000);
    }
    registerRxHandlerCA(C2AD_PING, doNothingCA); // TODO
    registerRxHandlerCA(C2AD_PING, NULL);
    printf("OK !\n");

    registerRxHandlerCA(A2CD_ERR, onA2CD_ERR);
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
    printDataCA(&data, sizeof(data));
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
        printDataCA(data, size);
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
    printDataCA(&c, sizeof(c));
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
    printDataCA(data, size);
#endif
}
