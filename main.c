#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

int main()
{
    HEXAGON currentHexagon;

    unsigned long lastReportAt = 0;

    for(unsigned long i = 0; i < TOTAL_HEXAGONS_WITH_LFET_RED_LOCKED; i++)
    {
        longToHexagon(i, &currentHexagon, false);

        if((clock() > CLOCKS_PER_SEC) && (lastReportAt < (clock() - CLOCKS_PER_SEC)))
        {
            printf("Current hexagon no. %lu\r\n", i);
            printf("%f%% of all hexagons tried\r\n", (float)i / (float)TOTAL_HEXAGONS_WITH_LFET_RED_LOCKED);
            printHexagon(&currentHexagon);
            lastReportAt = clock();
        }
    }

    return EXIT_SUCCESS;
}
