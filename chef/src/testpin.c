/* Teste si une broche est connecté à une autre */

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>

char testPin(char input, char output) {
    pinMode(output, OUTPUT);
    pinMode(input, INPUT);
    digitalWrite(output, 1);
    delay(0);
    return digitalRead(input);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s INPUT_PIN OUTPUT_PIN\n", argv[0]);
        return 2;
    }

    if (wiringPiSetup() == -1) {
        return 3;
    }

    char input = atoi(argv[1]);
    char output = atoi(argv[2]);

    char rep = testPin(input, output);

    return rep;
}
