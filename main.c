#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hexagon.h"

int main()
{
    HEXAGON printMe;
    for(int i = 0; i < 5; i++)
    {
        if(i < 3)
        {
            printMe.row0[i] = printMe.row4[i] = RED;
        }
        if(i < 4)
        {
            printMe.row1[i] = printMe.row3[i] = GREEN;
        }
        printMe.row2[i] = BLUE;
    }

    unsigned long i = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(1)
    {
        printf("%lu\r\n", i);
        longToHexagon(i, &printMe, false);
        printHexagon(&printMe);
        getchar();
        i++;
    }
#pragma clang diagnostic pop
}
