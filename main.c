#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

int main()
{
    HEXAGON currentHexagon;

//    for(int i = 0; i < TOTAL_SEGMENTS; i++)
//    {
//        unsigned long solution = 0;
//        for(int j = 0; j < cellRelationships[i].countOfRelationships; j++)
//        {
//            solution |= (unsigned long)YELLOW << cellRelationships[i].relatedCells[j] * 2;
//            longToHexagon(solution, &currentHexagon, false);
//        }
//        printf("%i:\r\n", i);
//        printHexagon(&currentHexagon);
//        getchar();
//    }

    unsigned long lastReportAt = 0;

    for(unsigned long i = 0; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i++)
    {
        if(validateSolution(i))
        {
            printf("Solution found at hexagon no. %lu\r\n", i);
            printf("%f%% of all hexagons tried\r\n", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            longToHexagon(i, &currentHexagon, false);
            printHexagon(&currentHexagon);
            return EXIT_SUCCESS;
        }

        if((clock() > CLOCKS_PER_SEC) && (lastReportAt < (clock() - CLOCKS_PER_SEC)))
        {
            printf("Current hexagon no. %lu\r\n", i);
            printf("%f%% of all hexagons tried\r\n", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            longToHexagon(i, &currentHexagon, false);
            printHexagon(&currentHexagon);
            lastReportAt = clock();
        }
    }

    return EXIT_FAILURE;
}
