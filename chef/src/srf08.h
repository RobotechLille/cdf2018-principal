#ifndef __SRF08_H__
#define __SRF08_H__

#include <stdint.h>

int openSRF08(int address);
uint8_t revSRF08(int fd);
void startSRF08(int fd);
void waitSRF08(int fd);
// In mm
float getSRF08(int fd);
float readSRF08(int fd);

#define SRF08_REGISTER_READ_REVISION 0x00
#define SRF08_REGISTER_READ_ECHO_1_HIGH 0x02
#define SRF08_REGISTER_READ_ECHO_1_LOW 0x03

#define SRF08_REGISTER_WRITE_COMMAND 0x00
#define SRF08_REGISTER_WRITE_MAX_GAIN 0x01
#define SRF08_REGISTER_WRITE_RANGE 0x02

#define SRF08_COMMAND_RANGING_IN 0x50
#define SRF08_COMMAND_RANGING_CM 0x51
#define SRF08_COMMAND_RANGING_US 0x52

#define SRF08_DEFAULT_MAX_GAIN 0x1F
#define SRF08_DEFAULT_RANGE 0xFF

#define SRF08_VALUES_OFFSET 0x80

#define SOUNDSPEED_MM_P_US 0.3312

#endif
