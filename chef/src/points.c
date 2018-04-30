#include <stdio.h>

#include "lcd.h"

int points;

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
    printfToLCD(LCD_LINE_2, "%d points", getPoints());
}

void addPoints(int pts)
{
    points += pts;
    showPoints();
}
