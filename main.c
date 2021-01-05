#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

int main(int argc, char ** argv)
{
    opterr = true;

    int option;

    bool printHexagons = false;

    while((option = getopt(argc, argv, "hp")) != -1)
    {
        switch(option)
        {
            case 'h':
                printf("Usage: hexxer [options]\r\n");
                printf("  -h  Display this help.\r\n");
                printf("  -p  Render and print all discovered solutions.\r\n");
                return EXIT_SUCCESS;

            case 'p':
                printHexagons = true;
                break;

            case '?':
            default:
                return EXIT_FAILURE;
        }
    }

    HEXAGON_AS_INT solutionsFound = 0;
    HEXAGON renderedHexagon;

    for(HEXAGON_AS_INT i = 0; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i++)
    {
        if(validateSolution(i))
        {
            solutionsFound++;
            printf("Solution found at hexagon no. %lu ", i);
            printf("%f%% of all hexagons tried, ", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            printf("%lu solutions found so far.\r\n", solutionsFound);
            if(printHexagons)
            {
                longToHexagon(i, &renderedHexagon, false);
                printHexagon(&renderedHexagon);
            }
        }
    }

    printf("Tried all possible hexagons.\r\n");

    return EXIT_SUCCESS;
}
