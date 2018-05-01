/*
 * Définition des signaux échagés entre l'Arduino et le chef
 */

#ifndef __CFSIGNALS_H_
#define __CFSIGNALS_H_

#include <stdint.h>

// Structures used everywhere

// D: Direct
// Sens naturel : Envoi de donnée
// Sens inverse : Accusé de réception

// I: Indirect
// Sens inverse : Demande de donnée
// Sens naturel : Envoi de donnée

// T: Trigger
// Sens inverse : Paramètrage du trigger
// Sens naturel : Envoi de trigger

// Arduino → FPGA

// Pour le debug
#define C2FD_PING 'P'

// FPGA → Arduino

// Erreur quelconque
#define F2CD_ERR 'E'
struct __attribute__ ((packed)) F2CD_ERRs {
    unsigned char code;
};

#define ERR_UNKNOWN_CODE 'C'

// Récupère les valeur des encodeurs
#define F2CI_CODER 'D'
struct __attribute__ ((packed)) F2CI_CODERs {
    int16_t dL;
    int16_t dR;
};

// Récupère les valeur des capteurs de distance
#define F2CI_CAPT 'C'
struct __attribute__ ((packed)) F2CI_CAPTs {
    uint16_t front;
    uint16_t back;
};

// Récupère les valeur des capteurs de distance (trigger au supérieur)
#define F2CT_CAPT 'c'
// /!\ Structure de la requête de trigger (A→F). Les données seront envoyées avec F2CI_CAPT 
struct __attribute__ ((packed)) F2CT_CAPTs { 
    uint16_t front;
    uint16_t back;
};

// Met à jour la PWM (pour ctrl moteur (EN, IN × 2) × 2)
#define C2FD_PWM 'W'
struct __attribute__ ((packed)) C2FD_PWMs { 
    uint8_t ena;
    uint8_t enb;
    uint8_t in;
};

// Met à jour la PWM (pour ctrl moteur (EN × 2) × 2)
#define C2FD_PWM2 'w'
struct __attribute__ ((packed)) C2FD_PWM2s { 
    uint8_t ena;
    uint8_t enc;
    uint8_t enb;
    uint8_t end;
};

#endif
