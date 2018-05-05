#include "imu.h"
#include "i2c.h"

int gyroFd;
int accelFd;

void configureIMU()
{
    gyroFd = openI2C(GYRO_ADDR);
    accelFd = openI2C(ACCEL_ADDR);

    writeI2C(gyroFd, GYRO_LOW_ODR, 0x00); // Low_ODR = 0 (low speed ODR disabled)
    writeI2C(gyroFd, GYRO_CTRL_REG4, 0x00); // FS = 00 (+/- 250 dps full scale)
    writeI2C(gyroFd, GYRO_CTRL_REG1, 0x6F); // DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
}

bool connectedIMU()
{
    return (uint8_t) readI2C(gyroFd, GYRO_WHO_AM_I) == GYRO_IDENTITY;
}

struct gyroRaw readGyroRaw()
{
    struct gyroRaw res;
    res.x = (readI2C(gyroFd, GYRO_OUT_X_H) << 8 | readI2C(gyroFd, GYRO_OUT_X_L));
    res.y = (readI2C(gyroFd, GYRO_OUT_Y_H) << 8 | readI2C(gyroFd, GYRO_OUT_Y_L));
    res.z = (readI2C(gyroFd, GYRO_OUT_Z_H) << 8 | readI2C(gyroFd, GYRO_OUT_Z_L));
    return res;
}

void deconfigureIMU()
{

}
