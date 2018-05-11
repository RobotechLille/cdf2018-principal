#include <stdlib.h>
#include <stdio.h>
#include "fpga.h"

int fpgaFd = -1;

int fdFPGA()
{
    if (fpgaFd < 0) {
        fpgaFd = openI2C(FPGA_ADDRESS);
        if (fpgaFd < 0) {
            perror("fdFPGA.openI2C");
        }
    }
    return fpgaFd;
}

bool connectedFPGA()
{
    return readI2C(fdFPGA(), FPGA_WHO_AM_I) == FPGA_IDENTITY;
}
