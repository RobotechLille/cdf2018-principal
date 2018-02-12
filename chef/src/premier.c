#include "premier.h"

#include "serial.h"

int main()
{
    unsigned char g;
    configureCA();
    sendByteCA('c');
    while ((g = readByteCA())) {
        printf("%c\n", g);
    }
    deconfigureCA();
    return 0;
}
