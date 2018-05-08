#ifndef __IMU_H_
#define __IMU_H_

#include <stdbool.h>
#include <stdint.h>

#define GYRO_ADDR 0x6b
#define ACEL_ADDR 0x1d
#define IMU_INTERVAL 10
#define COMPLEM_FILTER_RATE 0.98
// ms (100 Hz)

// Gyro

#define GYRO_GAIN 0.07
#define GYRO_WHO_AM_I 0x0F
#define GYRO_IDENTITY 0xD7

#define GYRO_CTRL1 0x20     // D20H
#define GYRO_CTRL_REG1 0x20 // D20, 4200D
#define GYRO_CTRL2 0x21     // D20H
#define GYRO_CTRL_REG2 0x21 // D20, 4200D
#define GYRO_CTRL3 0x22     // D20H
#define GYRO_CTRL_REG3 0x22 // D20, 4200D
#define GYRO_CTRL4 0x23     // D20H
#define GYRO_CTRL_REG4 0x23 // D20, 4200D
#define GYRO_CTRL5 0x24     // D20H
#define GYRO_CTRL_REG5 0x24 // D20, 4200D
#define GYRO_REFERENCE 0x25
#define GYRO_OUT_TEMP 0x26
#define GYRO_STATUS 0x27     // D20H
#define GYRO_STATUS_REG 0x27 // D20, 4200D

#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2A
#define GYRO_OUT_Y_H 0x2B
#define GYRO_OUT_Z_L 0x2C
#define GYRO_OUT_Z_H 0x2D

#define GYRO_FIFO_CTRL 0x2E     // D20H
#define GYRO_FIFO_CTRL_REG 0x2E // D20, 4200D
#define GYRO_FIFO_SRC 0x2F      // D20H
#define GYRO_FIFO_SRC_REG 0x2F  // D20, 4200D

#define GYRO_IG_CFG 0x30        // D20H
#define GYRO_INT1_CFG 0x30      // D20, 4200D
#define GYRO_IG_SRC 0x31        // D20H
#define GYRO_INT1_SRC 0x31      // D20, 4200D
#define GYRO_IG_THS_XH 0x32     // D20H
#define GYRO_INT1_THS_XH 0x32   // D20, 4200D
#define GYRO_IG_THS_XL 0x33     // D20H
#define GYRO_INT1_THS_XL 0x33   // D20, 4200D
#define GYRO_IG_THS_YH 0x34     // D20H
#define GYRO_INT1_THS_YH 0x34   // D20, 4200D
#define GYRO_IG_THS_YL 0x35     // D20H
#define GYRO_INT1_THS_YL 0x35   // D20, 4200D
#define GYRO_IG_THS_ZH 0x36     // D20H
#define GYRO_INT1_THS_ZH 0x36   // D20, 4200D
#define GYRO_IG_THS_ZL 0x37     // D20H
#define GYRO_INT1_THS_ZL 0x37   // D20, 4200D
#define GYRO_IG_DURATION 0x38   // D20H
#define GYRO_INT1_DURATION 0x38 // D20, 4200D

#define GYRO_LOW_ODR 0x39 // D20H

// Aceletomètre

#define ACEL_IDENTITY 0x49

#define ACEL_TEMP_OUT_L 0x05 // D
#define ACEL_TEMP_OUT_H 0x06 // D

#define ACEL_STATUS_M 0x07 // D

#define ACEL_INT_CTRL_M 0x12  // D
#define ACEL_INT_SRC_M 0x13   // D
#define ACEL_INT_THS_L_M 0x14 // D
#define ACEL_INT_THS_H_M 0x15 // D

#define ACEL_OFFSET_X_L_M 0x16 // D
#define ACEL_OFFSET_X_H_M 0x17 // D
#define ACEL_OFFSET_Y_L_M 0x18 // D
#define ACEL_OFFSET_Y_H_M 0x19 // D
#define ACEL_OFFSET_Z_L_M 0x1A // D
#define ACEL_OFFSET_Z_H_M 0x1B // D
#define ACEL_REFERENCE_X 0x1C  // D
#define ACEL_REFERENCE_Y 0x1D  // D
#define ACEL_REFERENCE_Z 0x1E  // D

#define ACEL_CTRL0 0x1F             // D
#define ACEL_CTRL1 0x20             // D
#define ACEL_CTRL_REG1_A 0x20       // DLH DLM DLHC
#define ACEL_CTRL2 0x21             // D
#define ACEL_CTRL_REG2_A 0x21       // DLH DLM DLHC
#define ACEL_CTRL3 0x22             // D
#define ACEL_CTRL_REG3_A 0x22       // DLH DLM DLHC
#define ACEL_CTRL4 0x23             // D
#define ACEL_CTRL_REG4_A 0x23       // DLH DLM DLHC
#define ACEL_CTRL5 0x24             // D
#define ACEL_CTRL_REG5_A 0x24       // DLH DLM DLHC
#define ACEL_CTRL6 0x25             // D
#define ACEL_CTRL_REG6_A 0x25       // DLHC
#define ACEL_HP_FILTER_RESET_A 0x25 // DLH DLM
#define ACEL_CTRL7 0x26             // D
#define ACEL_REFERENCE_A 0x26       // DLH DLM DLHC
#define ACEL_STATUS_A 0x27          // D
#define ACEL_STATUS_REG_A 0x27      // DLH DLM DLHC

#define ACEL_OUT_X_L_A 0x28
#define ACEL_OUT_X_H_A 0x29
#define ACEL_OUT_Y_L_A 0x2A
#define ACEL_OUT_Y_H_A 0x2B
#define ACEL_OUT_Z_L_A 0x2C
#define ACEL_OUT_Z_H_A 0x2D

#define ACEL_FIFO_CTRL 0x2E       // D
#define ACEL_FIFO_CTRL_REG_A 0x2E // DLHC
#define ACEL_FIFO_SRC 0x2F        // D
#define ACEL_FIFO_SRC_REG_A 0x2F  // DLHC

#define ACEL_IG_CFG1 0x30         // D
#define ACEL_INT1_CFG_A 0x30      // DLH DLM DLHC
#define ACEL_IG_SRC1 0x31         // D
#define ACEL_INT1_SRC_A 0x31      // DLH DLM DLHC
#define ACEL_IG_THS1 0x32         // D
#define ACEL_INT1_THS_A 0x32      // DLH DLM DLHC
#define ACEL_IG_DUR1 0x33         // D
#define ACEL_INT1_DURATION_A 0x33 // DLH DLM DLHC
#define ACEL_IG_CFG2 0x34         // D
#define ACEL_INT2_CFG_A 0x34      // DLH DLM DLHC
#define ACEL_IG_SRC2 0x35         // D
#define ACEL_INT2_SRC_A 0x35      // DLH DLM DLHC
#define ACEL_IG_THS2 0x36         // D
#define ACEL_INT2_THS_A 0x36      // DLH DLM DLHC
#define ACEL_IG_DUR2 0x37         // D
#define ACEL_INT2_DURATION_A 0x37 // DLH DLM DLHC

#define ACEL_CLICK_CFG 0x38      // D
#define ACEL_CLICK_CFG_A 0x38    // DLHC
#define ACEL_CLICK_SRC 0x39      // D
#define ACEL_CLICK_SRC_A 0x39    // DLHC
#define ACEL_CLICK_THS 0x3A      // D
#define ACEL_CLICK_THS_A 0x3A    // DLHC
#define ACEL_TIME_LIMIT 0x3B     // D
#define ACEL_TIME_LIMIT_A 0x3B   // DLHC
#define ACEL_TIME_LATENCY 0x3C   // D
#define ACEL_TIME_LATENCY_A 0x3C // DLHC
#define ACEL_TIME_WINDOW 0x3D    // D
#define ACEL_TIME_WINDOW_A 0x3D  // DLHC

#define ACEL_Act_THS 0x3E // D
#define ACEL_Act_DUR 0x3F // D

#define ACEL_CRA_REG_M 0x00 // DLH DLM DLHC
#define ACEL_CRB_REG_M 0x01 // DLH DLM DLHC
#define ACEL_MR_REG_M 0x02  // DLH DLM DLHC

#define ACEL_SR_REG_M 0x09  // DLH DLM DLHC
#define ACEL_IRA_REG_M 0x0A // DLH DLM DLHC
#define ACEL_IRB_REG_M 0x0B // DLH DLM DLHC
#define ACEL_IRC_REG_M 0x0C // DLH DLM DLHC

#define ACEL_WHO_AM_I 0x0F   // D
#define ACEL_WHO_AM_I_M 0x0F // DLM

#define ACEL_TEMP_OUT_H_M 0x31 // DLHC
#define ACEL_TEMP_OUT_L_M 0x32 // DLHC

#define ACEL_D_OUT_X_L_M 0x08
#define ACEL_D_OUT_X_H_M 0x09
#define ACEL_D_OUT_Y_L_M 0x0A
#define ACEL_D_OUT_Y_H_M 0x0B
#define ACEL_D_OUT_Z_L_M 0x0C
#define ACEL_D_OUT_Z_H_M 0x0D

// Structures

struct axesRaw {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct axes {
    float x;
    float y;
    float z;
};

void configureIMU();
bool connectedIMU();
void deconfigureIMU();
struct axesRaw readGyroRaw();
struct axesRaw readAcelRaw();
void* TaskIMU(void* pData);

#endif
