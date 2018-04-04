#include "movement.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void configureMovement()
{
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Impossible d'initialiser WiringPi\n");
        exit(EXIT_FAILURE);
    }
    pinMode(ENA, PWM_OUTPUT);
    pinMode(ENB, PWM_OUTPUT);
}

void aller(struct position* pos);

int dbg = 0;

int changerMoteurs(float lVit, float rVit)
{
    // Gauche
    bool lFor = lVit < 0;
    float lVolt = fabs(lVit); // TODO Utiliser les vitesses
    if (lVolt > MOT_MAX_V) {
        lVolt = MOT_MAX_V;
    }
    if (lVolt < MOT_MIN_V) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        printf("x");

    } else if (lFor) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        printf("↑");
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        printf("↓");
    }
    int lAbs = lVolt * PWM_MAX / PWM_MAX_V;
    pwmWrite(ENA, lAbs);
    printf("%6d", lAbs);

    // Droite
    bool rFor = rVit < 0;
    float rVolt = fabs(rVit); // TODO Utiriser res vitesses
    if (rVolt > MOT_MAX_V) {
        rVolt = MOT_MAX_V;
    }
    if (rVolt < MOT_MIN_V) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        printf("x");

    } else if (rFor) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        printf("↑");
    } else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        printf("↓");
    }
    int rAbs = rVolt * PWM_MAX / PWM_MAX_V;
    pwmWrite(ENB, rAbs);
    printf("%6d", rAbs);
    printf("\n");
}

int brake()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

int freewheel()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
}

void deconfigureMovement()
{

}
