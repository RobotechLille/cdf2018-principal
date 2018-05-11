#ifndef __FPGA_H__
#define __FPGA_H__

#include <stdbool.h>
#include "i2c.h"

#define FPGA_ADDRESS 0x42


#define FPGA_WHO_AM_I 0x00
#define FPGA_IDENTITY 0x50

// Read
#define CODER_LEFT_H 0x10
#define CODER_LEFT_L 0x11
#define CODER_RIGHT_H 0x12
#define CODER_RIGHT_L 0x13
#define CAPT_FRONT_LEFT_H_RAW 0x20
#define CAPT_FRONT_LEFT_L_RAW 0x21
#define CAPT_FRONT_RIGHT_H_RAW 0x22
#define CAPT_FRONT_RIGHT_L_RAW 0x23
#define CAPT_BACK_LEFT_H_RAW 0x24
#define CAPT_BACK_LEFT_L_RAW 0x25
#define CAPT_BACK_RIGHT_H_RAW 0x26
#define CAPT_BACK_RIGHT_L_RAW 0x27
#define CAPT_FRONT_LEFT_H 0x30
#define CAPT_FRONT_LEFT_L 0x31
#define CAPT_FRONT_RIGHT_H 0x32
#define CAPT_FRONT_RIGHT_L 0x33
#define CAPT_BACK_LEFT_H 0x34
#define CAPT_BACK_LEFT_L 0x35
#define CAPT_BACK_RIGHT_H 0x36
#define CAPT_BACK_RIGHT_L 0x37

// Write
#define MOTOR_IN 0x60
#define MOTOR_ENA 0x61
#define MOTOR_ENB 0x62

int fdFPGA();
bool connectedFPGA();

#endif
