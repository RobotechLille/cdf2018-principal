#include <unistd.h>

#include "i2c.h"
#include "srf08.h"

int openSRF08(int address)
{
    int fd = openI2C(address);
    writeI2C(fd, SRF08_REGISTER_WRITE_MAX_GAIN, SRF08_DEFAULT_MAX_GAIN);
    writeI2C(fd, SRF08_REGISTER_WRITE_RANGE, SRF08_DEFAULT_RANGE);
    return fd;
}

uint8_t revSRF08(int fd)
{
    return readI2C(fd, SRF08_REGISTER_READ_REVISION);
}

void startSRF08(int fd)
{
    writeI2C(fd, SRF08_REGISTER_WRITE_COMMAND, SRF08_COMMAND_RANGING_US);
}

void waitSRF08(int fd)
{
    while (revSRF08(fd) == 0xFF) {
        sleep(0);
    }
}

float getSRF08(int fd)
{
    int8_t high = readI2C(fd, SRF08_REGISTER_READ_ECHO_1_HIGH) - SRF08_VALUES_OFFSET;
    int8_t low = readI2C(fd, SRF08_REGISTER_READ_ECHO_1_LOW) - SRF08_VALUES_OFFSET;
    int total = high * SRF08_VALUES_OFFSET + low;
    return total * SOUNDSPEED_MM_P_US;
}

float readSRF08(int fd)
{
    startSRF08(fd);
    waitSRF08(fd);
    return getSRF08(fd);
}
