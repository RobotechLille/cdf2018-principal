#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // random seed
#include <unistd.h> // sleep

pthread_mutex_t posConnu;
pthread_cond_t newPos;
pthread_t tPosition;
pthread_t tMovement;

void* TaskPosition(void* pData)
{

    (void)pData;
    for (;;) {
        printf("Begin position\n");
        sleep(1);

        pthread_mutex_lock(&posConnu);
        printf("Sending position\n");
        pthread_cond_signal(&newPos);
        pthread_mutex_unlock(&posConnu);
    }
    return NULL;
}

void* TaskMovement(void* pData)
{
    (void)pData;
    for (;;) {
        printf("Begin Movement\n");
        sleep(3);

        pthread_mutex_lock(&posConnu);
        printf("Waiting movement\n");
        pthread_cond_wait(&newPos, &posConnu);
        pthread_mutex_unlock(&posConnu);

    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&posConnu, NULL);
    pthread_cond_init(&newPos, NULL);
    pthread_create(&tPosition, NULL, TaskPosition, NULL);
    pthread_create(&tMovement, NULL, TaskMovement, NULL);
    sleep(300);
}
