// Robotech Lille 2017-2018

#include "actionneurs.h"
#include "CA.h"

pthread_mutex_t receptionActionMutex;
pthread_cond_t receptionActionCond;

// On fait un truc de malpropre : étant donné que l'Arduino
// peut faire qu'une seule tache à la fois on suppose que le
// caractère reçu répond à celui qu'on attendait

void configureActionneurs()
{
    pthread_mutex_init(&receptionActionMutex, NULL);
    pthread_cond_init(&receptionActionCond, NULL);
}

void setLoquet(bool state)
{
    attendAction(state ? C2AD_OUVRE_LOQUET : C2AD_FERME_LOQUET);
}

void setPositionBalle(enum positionBalle pos)
{
    unsigned char code;
    switch (pos) {
        case attente:
            code = C2AD_POS_BALLE_ATTENTE;
            break;
        case ejection:
            code = C2AD_POS_BALLE_EJECTION;
            break;
        case evacuation:
            code = C2AD_POS_BALLE_EVACUATION;
            break;
    }
    attendAction(code);
}

void pousserBalle()
{
    attendAction(C2AD_POUSSER_BALLE);
}

void barilletSuivant()
{
    attendAction(C2AD_BARILLET_SUIVANT);
}

void barilletSkip()
{
    attendAction(C2AD_BARILLET_SKIP);
}

void barilletReset()
{
    attendAction(C2AD_BARILLET_RESET);
}

void setPropulsion(bool state)
{
    attendAction(state ? C2AD_PROPULSION_ON : C2AD_PROPULSION_OFF);
}


void resetActionneurs()
{
    setPropulsion(false);
    barilletReset();
}

void stopActionneurs()
{
    setPropulsion(false);
}

void receptionAction()
{
    pthread_mutex_lock(&receptionActionMutex);
    pthread_cond_signal(&receptionActionCond);
    pthread_mutex_unlock(&receptionActionMutex);
}

void attendAction(unsigned char code)
{
    pthread_mutex_lock(&receptionActionMutex);
    registerRxHandlerCA(code, receptionAction);
    sendCA(code, NULL, 0);
    pthread_cond_wait(&receptionActionCond, &receptionActionMutex);
    pthread_mutex_unlock(&receptionActionMutex);
}

void deconfigureActionneurs()
{
    stopActionneurs();
}
