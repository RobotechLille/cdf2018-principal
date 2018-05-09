#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float angleGap(float target, float actual)
{
    return remainderf(target - actual + M_PI, 2 * M_PI) - M_PI;
}

int main()
{
    float cons = 2;
    for (float conn = 0; conn < 10.0; conn += 0.1) {
        printf("%6f\n", angleGap(cons, conn));
    }
}
