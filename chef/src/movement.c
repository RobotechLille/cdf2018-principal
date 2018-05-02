#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "movement.h"
#include "CF.h"

void configureMovement()
{
    stop();
}

void aller(struct position* pos);

int dbg = 0;

uint8_t tensionToPWM(float V)
{
    if (V >= PWM_MAX_V) {
        return PWM_MAX;
    } else if (V <= 0) {
        return 0;
    } else {
        return V * (float) PWM_MAX / (float) PWM_MAX_V;
    }
}

// Tension de PWM
// TODO Changer en tension de moteur
int setMoteurTension(float lVolt, float rVolt)
{
    static struct C2FD_PWMs msg;

    msg.in = 0x00;

    // TODO Protections

    // Gauche
    bool lFor = lVolt < 0;
    lVolt = fabs(lVolt);
    msg.in |= 1 << (lFor ? IN1 : IN2);
    msg.ena = tensionToPWM(lVolt);

    // Droite
    bool rFor = rVolt < 0;
    rVolt = fabs(rVolt);
    msg.in |= 1 << (rFor ? IN3 : IN4);
    msg.enb = tensionToPWM(rVolt);

    sendCF(C2FD_PWM, &msg, sizeof(struct C2FD_PWMs));
}

int changerMoteurs(float lVit, float rVit)
{
    // TODO Conversion en vitesse
    setMoteurTension(lVit, rVit);
}

static struct C2FD_PWMs msgBrake = {0, 0, 0x00};
static struct C2FD_PWMs msgFree = {0, 0, (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4)};

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

void deconfigureMovement()
{
    stop();
}

