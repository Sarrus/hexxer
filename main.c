#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

int main()
{
    HEXAGON currentHexagon;

    //unsigned long lastReportAt = 0;

    unsigned long solutionsFound = 0;

    for(unsigned long i = 0; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i++)
    {
        if(validateSolution(i))
        {
            solutionsFound++;
            printf("Solution found at hexagon no. %lu ", i);
            printf("%f%% of all hexagons tried, ", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            printf("%lu solutions found so far.\r\n", solutionsFound);
            //longToHexagon(i, &currentHexagon, false);
            //printHexagon(&currentHexagon);
            //return EXIT_SUCCESS;
        }

//        if((clock() > CLOCKS_PER_SEC) && (lastReportAt < (clock() - CLOCKS_PER_SEC)))
//        {
//            //printf("Current hexagon no. %lu\r\n", i);
//            //printf("%f%% of all hexagons tried\r\n", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
//            longToHexagon(i, &currentHexagon, false);
//            printHexagon(&currentHexagon);
//            lastReportAt = clock();
//        }
    }

    printf("Tried all possible hexagons.\r\n");

    return EXIT_SUCCESS;
}
