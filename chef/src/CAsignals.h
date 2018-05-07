/*
 * Définition des signaux échagés entre l'Arduino et le chef
 */

#ifndef __CASIGNALS_H_
#define __CASIGNALS_H_

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

// Raspberry Pi → Arduino

// Pour le debug
#define C2AD_PING 'P'

#define C2AD_OUVRE_LOQUET 'L'

#define C2AD_FERME_LOQUET 'F'

#define C2AD_POS_BALLE_ATTENTE 'A'

#define C2AD_POS_BALLE_EVACUATION 'V'

#define C2AD_POS_BALLE_EJECTION 'J'

#define C2AD_POUSSER_BALLE 'O'

#define C2AD_BARILLET_SUIVANT 'B'

#define C2AD_BARILLET_SKIP 'H'

#define C2AD_BARILLET_RESET 'R'

#define C2AD_PROPULSION_ON 'T'

#define C2AD_PROPULSION_OFF 'U'

// Arduino → Raspberry Pi

#define A2CD_ERR 'E'

#define A2CD_ERR 'E'
struct __attribute__ ((packed)) A2CD_ERRs {
    unsigned char code;
};

#endif
