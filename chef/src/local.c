#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // sleep

#define TEMPS_PARCOURS 10

// t1 - t2
void diffTimespec(const struct timespec* t1, const struct timespec* t2, struct timespec* td)
{
    if ((t1->tv_nsec - t2->tv_nsec) < 0) {
        td->tv_sec = t1->tv_sec - t2->tv_sec - 1;
        td->tv_nsec = t1->tv_nsec - t2->tv_nsec + 1000000000UL;
    } else {
        td->tv_sec = t1->tv_sec - t2->tv_sec;
        td->tv_nsec = t1->tv_nsec - t2->tv_nsec;
    }
}

int main()
{
    struct timespec start, now, diff;
    clock_gettime(CLOCK_REALTIME, &start);

    for (;;) {
        clock_gettime(CLOCK_REALTIME, &now);
        diffTimespec(&now, &start, &diff);
        if (diff.tv_sec > TEMPS_PARCOURS) {
            break;
        }
        printf("32 %ld %ld\n", diff.tv_sec, diff.tv_nsec);
    }

    return EXIT_SUCCESS;
}
