#include "CF.h"
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

void configureFpga()
{
    // Connection au port série
    printf("Connexion à %s... ", FPGA_PORTNAME);
    fflush(stdout);
    fpga = open(FPGA_PORTNAME, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    fpga = serialOpen(FPGA_PORTNAME, 9600);
    if (fpga < 0) {
        printf("Échec !\n");
        exit(1);
    }

    // Configuration du port série
    fcntl(fpga, F_SETFL, O_RDWR);

    struct termios cfg;

    tcgetattr(fpga, &cfg);

    cfmakeraw(&cfg);
    cfsetispeed(&cfg, CF_BAUDRATE);
    cfsetospeed(&cfg, CF_BAUDRATE);

    cfg.c_cflag |= (CLOCAL | CREAD);
    cfg.c_cflag &= ~PARENB;
    cfg.c_cflag &= ~CSTOPB;
    cfg.c_cflag &= ~CSIZE;
    cfg.c_cflag |= CS8;
    cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    cfg.c_oflag &= ~OPOST;

    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 10;

    if (tcsetattr(fpga, TCSANOW, &cfg) < 0) {
        perror("serialConfig.tcsetattr");
        exit(1);
    }

    int status;
    ioctl(fpga, TIOCMGET, &status);

    status |= TIOCM_DTR;
    status |= TIOCM_RTS;

    ioctl(fpga, TIOCMSET, &status);

    usleep(10 * 1000);

    // Flush
    unsigned char trash[1024];
    read(fpga, &trash, sizeof(trash));

    printf("OK!\n");
}

void deconfigureFpga()
{
    close(fpga);
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
        code = readByteCF();

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

void onF2CD_ERR()
{
    struct F2CD_ERRs s;
    readCF(&s, sizeof(struct F2CD_ERRs));
    printf("Erreur reçue : %c (%2x)\n", s.code, s.code);
}

void setPret()
{
    pret = true;
}

void doNothing()
{
}

void configureCF()
{
    configureFpga();
    for (int i = 0; i < 256; i++) {
        rxHandlersAC[i] = NULL;
    }

    pthread_mutex_init(&sSendCF, NULL);
    pthread_create(&tReaderAC, NULL, TaskReaderAC, NULL);

    printf("Attente de réponse du Fpga... ");
    fflush(stdout);
    // Dans le cas où on aurait laissé l'Fpga en attente de donnée,
    // on envoie des pings en boucle jusqu'à ce qu'il nous réponde.
    pret = false;
    registerRxHandler(C2FD_PING, setPret);
    while (!pret) {
        sendCF(C2FD_PING, NULL, 0);
        usleep(100 * 1000);
    }
    registerRxHandler(C2FD_PING, doNothing); // TODO
    registerRxHandler(C2FD_PING, NULL);
    printf("OK !\n");

    registerRxHandler(F2CD_ERR, onF2CD_ERR);
}

void deconfigureCF()
{
    deconfigureFpga();
}

void sendByteCF(unsigned char data)
{
    write(fpga, &data, sizeof(data));

#ifdef PRINTRAWDATA
    printf("↑");
    printData(&data, sizeof(data));
#endif
}

void sendCF(unsigned char code, void* data, size_t size)
{
    pthread_mutex_lock(&sSendCF);

    sendByteCF(code);
    if (size > 0) {
        unsigned char* p = data;
        for (size_t i = 0; i < size; i++) {
            write(fpga, p, sizeof(unsigned char));
            p++;
        }
        // Envoyer plus d'un octet d'un coup curieusement il aime pas ça du tout
    }
    pthread_mutex_unlock(&sSendCF);

#ifdef PRINTRAWDATA
    if (size > 0) {
        printf("↑");
        printData(data, size);
    }
#endif
}

unsigned char readByteCF()
{
    unsigned char c;

    while (read(fpga, &c, sizeof(c)) < 1) {
        sleep(0);
    }
    return c;

#ifdef PRINTRAWDATA
    printf("↓");
    printData(&c, sizeof(c));
#endif
}

void readCF(void* data, size_t size)
{
    size_t remaining = size;
    int justRead;
    char* p = data;
    do {
        justRead = read(fpga, p, remaining);
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
