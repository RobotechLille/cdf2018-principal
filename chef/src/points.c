#include <stdio.h>

#include "lcd.h"

int points;
char tempLine[16];

void resetPoints()
{
    points = 0;
}

int getPoints()
{
    return points;
}

void showPoints()
{
    sprintf(tempLine, "%d points", getPoints());
    lockLCD();
    gotoLCD(LCD_LINE_2);
    printLCD(tempLine);
    unlockLCD();
}

void addPoints(int pts)
{
    points += pts;
    showPoints();
}
