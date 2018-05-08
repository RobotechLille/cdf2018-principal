#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "i2c.h"
#include "imu.h"

int gyroFd;
int acelFd;
pthread_t tIMU;

void configureIMU()
{
    gyroFd = openI2C(GYRO_ADDR);
    acelFd = openI2C(ACEL_ADDR);

    writeI2C(gyroFd, GYRO_CTRL_REG1, 0x0F); // PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
    writeI2C(gyroFd, GYRO_CTRL_REG4, 0xB0); // Block data update, 2000 dps full scale

    writeI2C(acelFd, ACEL_CTRL1, 0x6F); // 50 Hz Acceleration data-rate; block data update; Zen = Yen = Xen = 1 (all axes enabled)
    writeI2C(acelFd, ACEL_CTRL2, 0x20); // +/- 16G full scale

    pthread_create(&tIMU, NULL, TaskIMU, NULL);
}

bool connectedIMU()
{
    return (uint8_t)readI2C(gyroFd, GYRO_WHO_AM_I) == GYRO_IDENTITY && (uint8_t)readI2C(acelFd, ACEL_WHO_AM_I) == ACEL_IDENTITY;
}

struct axesRaw readGyroRaw()
{
    struct axesRaw res;
    res.x = (readI2C(gyroFd, GYRO_OUT_X_H) << 8 | readI2C(gyroFd, GYRO_OUT_X_L));
    res.y = (readI2C(gyroFd, GYRO_OUT_Y_H) << 8 | readI2C(gyroFd, GYRO_OUT_Y_L));
    res.z = (readI2C(gyroFd, GYRO_OUT_Z_H) << 8 | readI2C(gyroFd, GYRO_OUT_Z_L));
    return res;
}

struct axes readGyro()
{
    struct axesRaw raw = readGyroRaw();
    struct axes computed;
    computed.x = (float)raw.x * GYRO_GAIN;
    computed.y = (float)raw.y * GYRO_GAIN;
    computed.z = (float)raw.z * GYRO_GAIN;
    return computed;
}

struct axesRaw readAcelRaw()
{
    struct axesRaw res;
    res.x = (readI2C(acelFd, ACEL_OUT_X_H_A) << 8 | readI2C(acelFd, ACEL_OUT_X_L_A));
    res.y = (readI2C(acelFd, ACEL_OUT_Y_H_A) << 8 | readI2C(acelFd, ACEL_OUT_Y_L_A));
    res.z = (readI2C(acelFd, ACEL_OUT_Z_H_A) << 8 | readI2C(acelFd, ACEL_OUT_Z_L_A));
    return res;
}

struct axes readAcel()
{
    struct axesRaw raw = readAcelRaw();
    struct axes computed;
    computed.x = atan2(raw.y, raw.z) + M_PI;
    computed.y = atan2(raw.z, raw.x) + M_PI;
    computed.z = atan2(raw.x, raw.y) + M_PI;
    return computed;
}

struct timespec imuStart;
struct timespec imuNow;
struct timespec imuEcoule;

void* TaskIMU(void* pData)
{
    (void)pData;

    struct axes gyroNew;
    struct axes gyro;
    struct axes acel;
    struct axes complemFilter;

    clock_gettime(CLOCK_REALTIME, &imuStart);

    for (;;) {
        clock_gettime(CLOCK_REALTIME, &imuNow);
        if ((imuNow.tv_nsec - imuStart.tv_nsec) < 0) {
            imuEcoule.tv_sec = imuNow.tv_sec - imuStart.tv_sec - 1;
            imuEcoule.tv_nsec = imuNow.tv_nsec - imuStart.tv_nsec + 1000000000UL;
        } else {
            imuEcoule.tv_sec = imuNow.tv_sec - imuStart.tv_sec;
            imuEcoule.tv_nsec = imuNow.tv_nsec - imuStart.tv_nsec;
        }
        imuStart.tv_sec = imuNow.tv_sec;
        imuStart.tv_nsec = imuNow.tv_nsec;
        float dt = imuEcoule.tv_sec + imuStart.tv_nsec * 1E-9;

        gyroNew = readGyro();
        gyro.x += gyroNew.x * dt;
        gyro.y += gyroNew.y * dt;
        gyro.z += gyroNew.z * dt;

        acel = readAcel();

        /* complemFilter.x = c * (complemFilter.x + gyroNew.x * dt) + (1 - AA) * acel.x; */

        usleep(IMU_INTERVAL * 1000);
    }

    return NULL;
}

void deconfigureIMU()
{
    pthread_cancel(tIMU);
}
