#include <stdio.h>
#include <stdlib.h>
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

    printHexagon(&printMe);

    return EXIT_SUCCESS;
}
