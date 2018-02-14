/*
 * Définition des signaux échagés entre l'Arduino et le chef
 */

#ifndef __ACSIGNALS_H_
#define __ACSIGNALS_H_

#define AC_BAUDRATE 9600UL

// D: Direct
// Sens naturel : Envoi de donnée
// Sens inverse : Accusé de réception

// I: Indirect
// Sens inverse : Demande de donnée
// Sens naturel : Envoi de donnée

// T: Trigger
// Sens inverse : Paramètrage du trigger
// Sens naturel : Envoi de trigger

// Chef → Arduino

// Pour le debug
#define C2AD_PING 'P'
struct C2AD_PINGs {
};

// Arrête tous les actionneurs
#define C2AD_STOP 'S'
struct C2ADD_STOPs {
};

// Donne une destination
#define C2AD_GOTO 'G'
struct C2AD_GOTOs {
    float x;
    float y;
    float o; // Peut-être NaN ou autre valeur spécifique si indifférent
};

// Arduino → Chef

// Erreur quelconque
#define A2CD_ERR 'E'
struct A2CI_ERRs {
    unsigned char code;
};

#define ERR_UNKNOWN_CODE 'C'

// Envoie les infos de debug
#define A2CI_DBG 'D'
struct A2CI_DBGs {
    float x;
    float y;
    float o;
    // ...
};


#endif
