#include <unistd.h>

#include "actionneurs.h"
#include "debug.h"
#include "motor.h"
#include "fpga.h"

float lVolt;
float rVolt;
float lVoltCons;
float rVoltCons;
struct timespec motStart;
struct timespec motNow;
struct timespec motEcoule;
pthread_mutex_t motCons;
pthread_t tMotor;
enum motorState motState = braking;

void configureMotor()
{
    registerDebugVar("lVolt", f, &lVolt);
    registerDebugVar("rVolt", f, &rVolt);
    pthread_mutex_init(&motCons, NULL);
    pthread_create(&tMotor, NULL, TaskMotor, NULL);
}

void deconfigureMotor()
{
    pthread_cancel(tMotor);
}

uint8_t tensionToPWM(float V)
{
    if (V >= PWM_MAX) {
        return UINT8_MAX;
    } else if (V <= 0) {
        return 0;
    } else {
        return V * UINT8_MAX / PWM_MAX;
    }
}

uint8_t moteurTensionToPWM(float V)
{
    if (V >= MOTOR_CONTROLLER_ALIMENTATION) {
        V = MOTOR_CONTROLLER_ALIMENTATION;
    } else if (V <= 0) {
        V = 0;
    }
    return tensionToPWM(V * MOTOR_CONTROLLER_REFERENCE / MOTOR_CONTROLLER_ALIMENTATION);
}

void setMoteurTensionRaw(float l, float r, bool lFor, bool rFor)
{
    lVolt = l;
    rVolt = r;

    uint8_t enA = 0;
    uint8_t enB = 0;
    uint8_t in = 0;

#ifdef INVERSE_L_MOTOR
    lFor = !lFor;
#endif
    if (l > 0) {
        in |= 1 << (lFor ? IN1 : IN2);
        enA = moteurTensionToPWM(l);
    } else {
        // Stay at 0 : brake mode
    }

#ifdef INVERSE_R_MOTOR
    rFor = !rFor;
#endif
    if (r > 0) {
        in |= 1 << (rFor ? IN3 : IN4);
        enB = moteurTensionToPWM(r);
    } else {
        // Stay at 0 : brake mode
    }

    writeI2C(fdFPGA(), MOTOR_IN, in);
    writeI2C(fdFPGA(), MOTOR_ENA, enA);
    writeI2C(fdFPGA(), MOTOR_ENB, enB);
}

void* TaskMotor(void* pData)
{
    (void)pData;

    float debug = 0;

    clock_gettime(CLOCK_REALTIME, &motStart);
    for (;;) {
        // Calcul du temps écoulé depuis la dernière mise à jour des moteurs
        clock_gettime(CLOCK_REALTIME, &motNow);
        if ((motNow.tv_nsec - motStart.tv_nsec) < 0) {
            motEcoule.tv_sec = motNow.tv_sec - motStart.tv_sec - 1;
            motEcoule.tv_nsec = motNow.tv_nsec - motStart.tv_nsec + 1000000000L;
        } else {
            motEcoule.tv_sec = motNow.tv_sec - motStart.tv_sec;
            motEcoule.tv_nsec = motNow.tv_nsec - motStart.tv_nsec;
        }
        motStart.tv_sec = motNow.tv_sec;
        motStart.tv_nsec = motNow.tv_nsec;
        float dt = motEcoule.tv_sec + (float) motEcoule.tv_nsec * 1E-9;
        debug += dt;

        pthread_mutex_lock(&motCons);
        bool lFor = lVoltCons < 0;
        float l = fabs(lVoltCons);
        bool rFor = rVoltCons < 0;
        float r = fabs(rVoltCons);
        enum motorState state = motState;
        pthread_mutex_unlock(&motCons);

        switch (state) {
        case running:

            if (l < MOTOR_SATURATION_MIN) {
                l = 0;
            } else if (l > MOTOR_SATURATION_MAX) {
                l = MOTOR_SATURATION_MAX;
            }

            if (r < MOTOR_SATURATION_MIN) {
                r = 0;
            } else if (r > MOTOR_SATURATION_MAX) {
                r = MOTOR_SATURATION_MAX;
            }

#ifdef ENABLE_RATE_LIMITER
            // Rate limiter pour éviter que l'accélération soit trop brusque
            float maxUp = RATE_LIMITER_UP * dt;
            float maxDown = RATE_LIMITER_UP * dt;

            if (l - lVolt > maxUp) {
                l = lVolt + maxUp;
            } else if (lVolt - l > maxDown) {
                l = lVolt - maxDown;
            }
            if (r - rVolt > maxUp) {
                r = rVolt + maxUp;
            } else if (rVolt - r > maxDown) {
                r = rVolt - maxDown;
            }
#endif

            setMoteurTensionRaw(l, r, lFor, rFor);
            break;
        case braking:
            rawBrake();
            break;
        case freewheeling:
            rawFreewheel();
            break;
        }

        usleep(1000);
    }

    return NULL;
}

void rawBrake()
{
    lVolt = 0;
    rVolt = 0;
    writeI2C(fdFPGA(), MOTOR_IN, 0);
    writeI2C(fdFPGA(), MOTOR_ENA, UINT8_MAX);
    writeI2C(fdFPGA(), MOTOR_ENB, UINT8_MAX);
}
void rawFreewheel()
{
    lVolt = 0;
    rVolt = 0;
    writeI2C(fdFPGA(), MOTOR_IN, (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4));
    writeI2C(fdFPGA(), MOTOR_ENA, 0);
    writeI2C(fdFPGA(), MOTOR_ENA, 0);
}

void setMoteurTension(float l, float r)
{
    pthread_mutex_lock(&motCons);
    motState = running;
    lVoltCons = l;
    rVoltCons = r;
    pthread_mutex_unlock(&motCons);
}

void setPWMTension(float lVolt, float rVolt)
{
    setMoteurTension(
        lVolt * MOTOR_CONTROLLER_ALIMENTATION / MOTOR_CONTROLLER_REFERENCE,
        rVolt * MOTOR_CONTROLLER_ALIMENTATION / MOTOR_CONTROLLER_REFERENCE);
}

int brake()
{
    pthread_mutex_lock(&motCons);
    motState = braking;
    pthread_mutex_unlock(&motCons);
    rawBrake();
}

int freewheel()
{
    pthread_mutex_lock(&motCons);
    motState = freewheeling;
    pthread_mutex_unlock(&motCons);
    rawFreewheel();
}

int stop()
{
    brake();
    stopActionneurs();
}
