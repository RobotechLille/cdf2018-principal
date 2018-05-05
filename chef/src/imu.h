#ifndef __IMU_H_
#define __IMU_H_

#include <stdbool.h>
#include <stdint.h>

#define GYRO_ADDR 0x6b
#define ACCEL_ADDR 0x1d

// Gyro

#define GYRO_WHO_AM_I 0x0F
#define GYRO_IDENTITY 0xD7

#define GYRO_CTRL1 0x20 // D20H
#define GYRO_CTRL_REG1 0x20 // D20, 4200D
#define GYRO_CTRL2 0x21 // D20H
#define GYRO_CTRL_REG2 0x21 // D20, 4200D
#define GYRO_CTRL3 0x22 // D20H
#define GYRO_CTRL_REG3 0x22 // D20, 4200D
#define GYRO_CTRL4 0x23 // D20H
#define GYRO_CTRL_REG4 0x23 // D20, 4200D
#define GYRO_CTRL5 0x24 // D20H
#define GYRO_CTRL_REG5 0x24 // D20, 4200D
#define GYRO_REFERENCE 0x25
#define GYRO_OUT_TEMP 0x26
#define GYRO_STATUS 0x27 // D20H
#define GYRO_STATUS_REG 0x27 // D20, 4200D

#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2A
#define GYRO_OUT_Y_H 0x2B
#define GYRO_OUT_Z_L 0x2C
#define GYRO_OUT_Z_H 0x2D

#define GYRO_FIFO_CTRL 0x2E // D20H
#define GYRO_FIFO_CTRL_REG 0x2E // D20, 4200D
#define GYRO_FIFO_SRC 0x2F // D20H
#define GYRO_FIFO_SRC_REG 0x2F // D20, 4200D

#define GYRO_IG_CFG 0x30 // D20H
#define GYRO_INT1_CFG 0x30 // D20, 4200D
#define GYRO_IG_SRC 0x31 // D20H
#define GYRO_INT1_SRC 0x31 // D20, 4200D
#define GYRO_IG_THS_XH 0x32 // D20H
#define GYRO_INT1_THS_XH 0x32 // D20, 4200D
#define GYRO_IG_THS_XL 0x33 // D20H
#define GYRO_INT1_THS_XL 0x33 // D20, 4200D
#define GYRO_IG_THS_YH 0x34 // D20H
#define GYRO_INT1_THS_YH 0x34 // D20, 4200D
#define GYRO_IG_THS_YL 0x35 // D20H
#define GYRO_INT1_THS_YL 0x35 // D20, 4200D
#define GYRO_IG_THS_ZH 0x36 // D20H
#define GYRO_INT1_THS_ZH 0x36 // D20, 4200D
#define GYRO_IG_THS_ZL 0x37 // D20H
#define GYRO_INT1_THS_ZL 0x37 // D20, 4200D
#define GYRO_IG_DURATION 0x38 // D20H
#define GYRO_INT1_DURATION 0x38 // D20, 4200D

#define GYRO_LOW_ODR 0x39 // D20H

// Structures

struct gyroRaw {
    int16_t x;
    int16_t y;
    int16_t z;
};

void configureIMU();
bool connectedIMU();
void deconfigureIMU();
struct gyroRaw readGyroRaw();

#endif
