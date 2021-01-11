//
// Created by Michael Bell on 11/01/2021.
//

#include <stdio.h>
#include "memory.h"
#include <pthread.h>
#include <stdlib.h>

#define SOLUTION_PAGE_SIZE 5

HEXAGON_AS_INT * solutions = NULL;
HEXAGON_AS_INT solutionPageCount = 0;
//pthread_mutex_t solutionsMutex;

void memoryInit()
{
    //pthread_mutex_init(&solutionsMutex, NULL);
    solutionsStored = 0;
}

void storeSolution(HEXAGON_AS_INT solution)
{
    //pthread_mutex_lock(&solutionsMutex);

    if(solutionsStored == solutionPageCount * SOLUTION_PAGE_SIZE)
    {
        solutionPageCount++;
        solutions = realloc(solutions, sizeof(HEXAGON_AS_INT) * SOLUTION_PAGE_SIZE * solutionPageCount);
        if(solutions == NULL)
        {
            printf("Memory allocation error.\r\n");
            exit(EXIT_FAILURE);
        }
    }

    solutions[solutionsStored] = solution;
    solutionsStored++;

    //pthread_mutex_unlock(&solutionsMutex);
}

HEXAGON_AS_INT retrieveSolution(HEXAGON_AS_INT solutionID)
{
    //pthread_mutex_lock(&solutionsMutex);

    HEXAGON_AS_INT solution = solutions[solutionID];

    //pthread_mutex_unlock(&solutionsMutex);

    return solution;
}