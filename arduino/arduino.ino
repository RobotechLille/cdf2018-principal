#include "Arduino.h"
#include "AX12A.h"
#include <Servo.h>

#define DirectionPin 2
#define BaudRate 1000000
#define ID 14

Servo servoLoquet;
Servo servoPositionBalle;
Servo servoPoussoir;

int pinservoLoquet = 7;
int pinservoPositionBalle = 8;
int pinservoPoussoir = 6;
int relai = 10;
int pos_initial = 512;
int pos_max = 960;
int pos_min = 40;
char lettre;

void setup()
{

    Serial.begin(9600);
    ax12a.begin(BaudRate, DirectionPin, &Serial1);
    servoLoquet.attach(pinservoLoquet);
    servoPositionBalle.attach(pinservoPositionBalle);
    servoPoussoir.attach(pinservoPoussoir);
    pinMode(relai, OUTPUT);
}

void barilletUnCran()
{

    ax12a.setEndless(ID, ON);
    ax12a.turn(ID, LEFT, 1220);
    delay(978);
    ax12a.turn(ID, LEFT, 200);
    delay(100);
    ax12a.turn(ID, LEFT, 1224);
    delay(100);
}

void barilletDeuxCrans()
{

    ax12a.setEndless(ID, ON);
    ax12a.turn(ID, LEFT, 1220);
    delay(1955);
    ax12a.turn(ID, LEFT, 200);
    delay(100);
    ax12a.turn(ID, LEFT, 1224);
    delay(100);
}

void loop()
{

    if (Serial.available()) { // Si le port reçoit une donnée

        lettre = Serial.read(); // On lit l'octet et on l'effacera

        bool codeInconnu = false;

        switch (lettre) {
        case 'L': // Ouverture loquet
            servoLoquet.write(100);
            delay(500);
            break;
        case 'F': // Fermeture loquet
            servoLoquet.write(0);
            delay(500);
            break;
        case 'A': // Position attente balle
            servoPositionBalle.write(70);
            //delay(2000);
            break;
        case 'V': // Position évacuation balle
            servoPositionBalle.write(0);
            //delay(2000);
            break;
        case 'J': // Position éjection balle
            servoPositionBalle.write(180);
            //delay(2000);
            break;
        case 'O': // Pousser balle
            // Position basse
            servoPoussoir.write(0);
            delay(1000);
            // Position haute
            servoPoussoir.write(120);
            delay(1000);
            break;
        case 'B': // Tourner barillet d'un cran
            barilletUnCran();
            break;
        case 'H': // Tourner de deux crans
            barilletDeuxCrans();
            break;
        case 'R': // Reset barillet
            // ax12a.setEndless(ID, OFF);
            ax12a.setEndless(ID, ON);
            ax12a.turn(ID, LEFT, 0);
            break;
        case 'T': // Propulsion on
            digitalWrite(relai, HIGH);
            break;
        case 'U': // Propulsion off
            digitalWrite(relai, LOW);
            break;
        case 'P': // Ping
            break;
        default:
            codeInconnu = true;
        }

        if (codeInconnu) {
            Serial.write('E');
            Serial.write('C');
        } else {
            Serial.write(lettre);
        }
    }
}
