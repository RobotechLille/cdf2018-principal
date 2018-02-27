/*
 * Définition des signaux échagés entre l'Arduino et le chef
 */

#ifndef __AFSIGNALS_H_
#define __AFSIGNALS_H_

#include <stdint.h>

#define AF_BAUDRATE 9600UL

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
#define A2FD_PING 'P'

// FPGA → Arduino

// Erreur quelconque
#define F2AD_ERR 'E'
struct __attribute__ ((packed)) F2AD_ERRs {
    unsigned char code;
};

#define ERR_UNKNOWN_CODE 'C'

// Récupère les valeur des encodeurs
#define F2AI_CODER 'D'
struct __attribute__ ((packed)) F2AI_CODERs {
    int16_t left;
    int16_t right;
};

// Récupère les valeur des capteurs de distance
#define F2AI_CAPT 'C'
struct __attribute__ ((packed)) F2AI_CAPTs {
    uint16_t front;
    uint16_t back;
};

// Récupère les valeur des capteurs de distance (trigger au supérieur)
#define F2AT_CAPT 'c'
// /!\ Structure de la requête de trigger (A→F). Les données seront envoyées avec F2AI_CAPT 
struct __attribute__ ((packed)) F2AT_CAPTs { 
    uint16_t front;
    uint16_t back;
};


#endif
