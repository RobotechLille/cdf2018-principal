#include <stdio.h>

#include "lcd.h"
#include "debug.h"

int points;

void configurePoints()
{
    registerDebugVar("points", d, &points);
}

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
