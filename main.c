#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hexagon.h"

const struct {
    char countOfRelationships;
    char relatedCells[6];
} cellRelationships[TOTAL_SEGMENTS] = {
        {3, {1, 3, 4}}, // 0
        {4, {0, 2, 4, 5}}, // 1
        {3, {1, 5, 6}}, // 2
        {4, {0, 4, 7, 8}}, // 3
        {6, {0, 1, 3,5,8,9}}, // 4
        {6, {1, 2, 4, 6, 9, 10}}, // 5
        {4, {2, 5, 10, 11}}, // 6
        {3, {3, 8, 12}}, // 7
        {6, {3, 4, 7, 9, 12, 13}}, // 8
        {6, {4, 5, 8, 10, 13, 14}}, // 9
        {6, {5, 6, 9, 11, 14, 15}}, // 10
        {3, {6, 10, 15}}, // 11
        {4, {7, 8, 13, 16}}, // 12
        {6, {8, 9, 12, 14, 16, 17}}, // 13
        {6, {9, 10, 13, 15, 17, 18}}, // 14
        {4, {10, 11, 14, 18}}, // 15
        {3, {12, 13, 17}}, // 16
        {4, {13, 14, 16, 18}}, // 17
        {3, {14, 15, 17}}
};


int main()
{
    HEXAGON currentHexagon;

    for(int i = 0; i < TOTAL_SEGMENTS; i++)
    {
        unsigned long solution = 0;
        for(int j = 0; j < cellRelationships[i].countOfRelationships; j++)
        {
            solution |= (unsigned long)YELLOW << cellRelationships[i].relatedCells[j] * 2;
            longToHexagon(solution, &currentHexagon, false);
        }
        printf("%i:\r\n", i);
        printHexagon(&currentHexagon);
        getchar();
    }

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
