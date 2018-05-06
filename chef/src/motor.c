#include "motor.h"

uint8_t tensionToPWM(float V)
{
    printf("PWM tension %f\n", V);
    if (V >= PWM_MAX) {
        return UINT8_MAX;
    } else if (V <= 0) {
        return 0;
    } else {
        printf("PWM value %d\n", (uint8_t) (V * UINT8_MAX / PWM_MAX));
        return V * UINT8_MAX / PWM_MAX;
    }
}

uint8_t moteurTensionToPWM(float V)
{
    printf("Moteur tension %f\n", V);
    if (V >= MOTOR_CONTROLLER_ALIMENTATION) {
        V = MOTOR_CONTROLLER_ALIMENTATION;
    } else if (V <= 0) {
        V = 0;
    }
    return tensionToPWM(V * MOTOR_CONTROLLER_REFERENCE / MOTOR_CONTROLLER_ALIMENTATION);
}

void setMoteurTensionRaw(float lVolt, float rVolt, bool lFor, bool rFor)
{

#ifdef INVERSE_L_MOTOR
    lFor = !lFor;
#endif
    static struct C2FD_PWMs msg;
    msg.in = 0x00;
    if (lVolt > 0) {
        msg.in |= 1 << (lFor ? IN1 : IN2);
        msg.ena = moteurTensionToPWM(lVolt);
    } else {
        // Nothing needs to be changed for this motor controller
    }

#ifdef INVERSE_R_MOTOR
    rFor = !rFor;
#endif
    if (rVolt > 0) {
        msg.in |= 1 << (rFor ? IN3 : IN4);
        msg.enb = moteurTensionToPWM(lVolt);
    } else {
        // Nothing needs to be changed for this motor controller
    }

    sendCF(C2FD_PWM, &msg, sizeof(struct C2FD_PWMs));
}

void setMoteurTension(float lVolt, float rVolt)
{

    // Gauche
    bool lFor = lVolt < 0;
    lVolt = fabs(lVolt);
    if (lVolt < MOTOR_SATURATION_MIN) {
        lVolt = 0;
    } else if (lVolt > MOTOR_SATURATION_MAX) {
        lVolt = MOTOR_SATURATION_MAX;
    }

    // Droite
    bool rFor = rVolt < 0;
    rVolt = fabs(rVolt);
    if (rVolt < MOTOR_SATURATION_MIN) {
        rVolt = 0;
    } else if (rVolt > MOTOR_SATURATION_MAX) {
        rVolt = MOTOR_SATURATION_MAX;
    }

    setMoteurTensionRaw(lVolt, rVolt, lFor, rFor);
}

void setPWMTension(float lVolt, float rVolt)
{
    setMoteurTension(
        lVolt * MOTOR_CONTROLLER_ALIMENTATION / MOTOR_CONTROLLER_REFERENCE,
        rVolt * MOTOR_CONTROLLER_ALIMENTATION / MOTOR_CONTROLLER_REFERENCE);
}

static struct C2FD_PWMs msgBrake = { 0, 0, 0x00 };
static struct C2FD_PWMs msgFree = { 0, 0, (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4) };

int brake()
{
    sendCF(C2FD_PWM, &msgBrake, sizeof(struct C2FD_PWMs));
}

int freewheel()
{
    sendCF(C2FD_PWM, &msgFree, sizeof(struct C2FD_PWMs));
}

int stop()
{
    brake();
    // TODO Actionneurs
}
