# Connexions

Ce document a pour but de recenser les connexions entre les différents composants du robot.

## Liste des composants

- Raspberry Pi 3 (RPi)
- Arduino Mega 2560 (Ard)
- Micronova Mercury FPGA (FPGA)
- HEDM-550X Encodeur moteur gauche
- HEDM-550X Encodeur moteur droit

## Connexions de puissance

## Connexions d'information

- BCM 2 (SDA), BCM 3 (SCL) : liaison I2C (LCD, IMU, SRF08)
- BCM 14 (TXD), BCM 15 (RXD) : liaison série (PC debug)
- BCM 12 (PWM0), BCM 13 (PWM1) : contrôleur moteur
- BCM 17, BCM 27 : bouton rouge, bouton jaune
- BCM 22 : tirette
