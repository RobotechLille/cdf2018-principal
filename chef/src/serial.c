#include "serial.h"
#include <stdio.h> // printf...
#include <stdlib.h> // stuff
#include <unistd.h> // read(), write()...
#include <fcntl.h> // O_*
#include <strings.h> // bzero

#include "common.h"

void configureCA()
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

    printf("OK!\n");
}

void deconfigureCA() {
    close(arduino);
}

void sendByteCA(unsigned char data) {
    write(arduino, &data, sizeof(data));
}

void sendCA(unsigned char code, void* data, size_t size)
{
    sendByteCA(code);
    write(arduino, data, size);
}

unsigned char readByteCA() {
    unsigned char c;
    while (read(arduino, &c, sizeof(c)) < 1) {
    }
    return c;
}
