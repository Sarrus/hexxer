#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

int main()
{
    HEXAGON_AS_INT solutionsFound = 0;

    for(HEXAGON_AS_INT i = 0; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i++)
    {
        if(validateSolution(i))
        {
            solutionsFound++;
            printf("Solution found at hexagon no. %lu ", i);
            printf("%f%% of all hexagons tried, ", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            printf("%lu solutions found so far.\r\n", solutionsFound);
        }
    }

    printf("Tried all possible hexagons.\r\n");

    return EXIT_SUCCESS;
}
