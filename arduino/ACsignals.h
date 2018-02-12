// Définition des signaux échagés entre l'Arduino et le chef
//

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
#define C2AD_PING 0
// Arrête tous les actionneurs
#define C2AD_STOP 1
// Donne une destination
#define C2AD_GOTO 2
// Donne une rotation
#define C2AD_ROTATE 3

// Arduino → Chef

// Envoie la position actuelle
#define A2CI_POS

#endif
