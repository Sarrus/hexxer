#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "hexagon.h"

#define MAX_PARALLEL_JOBS 256

bool printHexagons = false;
unsigned long parallelJobs = 0;

struct solverThreadConfig
{
    HEXAGON_AS_INT firstHexagon;
    HEXAGON_AS_INT currentHexagon;
    HEXAGON_AS_INT lastHexagon;
};

#define SOLVER_THREAD_CONFIG struct solverThreadConfig
SOLVER_THREAD_CONFIG threadConfigs[MAX_PARALLEL_JOBS];

HEXAGON_AS_INT solutionsFound = 0;

bool lastPrintWasProgressLine = false;

void solveInSerial()
{

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
}

HEXAGON_AS_INT printParallelProgress()
{
    HEXAGON_AS_INT hexagonsProcessed = 0;
    for(unsigned long i = 0; i < parallelJobs; i++)
    {
        hexagonsProcessed += threadConfigs[i].currentHexagon - threadConfigs[i].firstHexagon;
    }

    if(lastPrintWasProgressLine)
    {
        printf("%c[2K\r", 27);
    }

    printf(
            "%lu hexagons processed so far, %f%% of total.",
            hexagonsProcessed,
            100 * (float)hexagonsProcessed / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED
    );

    lastPrintWasProgressLine = true;

    return hexagonsProcessed;
}

void * solverThread(void * config)
{
    SOLVER_THREAD_CONFIG * solverConfig = config;
    HEXAGON renderedHexagon;

    for(
            solverConfig->currentHexagon = solverConfig->firstHexagon;
            solverConfig->currentHexagon < solverConfig->lastHexagon;
            solverConfig->currentHexagon++
            )
    {
        if(validateSolution(solverConfig->currentHexagon))
        {
            solutionsFound++;
            if(lastPrintWasProgressLine)
            {
                printf("%c[2K\r", 27);
                //printf("\r\n");
                lastPrintWasProgressLine = false;
            }
            printf("Solution found at hexagon no. %lu ", solverConfig->currentHexagon);
            printf("%lu solutions found so far.\r\n", solutionsFound);
            if(printHexagons)
            {
                longToHexagon(solverConfig->currentHexagon, &renderedHexagon, false);
                printHexagon(&renderedHexagon);
            }

            printParallelProgress();
        }
    }

    //printf("This solver's range is: %lu to %lu.\r\n", solverConfig->firstHexagon, solverConfig->lastHexagon);
    return NULL;
}

void * monitorThread(void * config)
{
    while(1)
    {


        if(printParallelProgress() == TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED)
        {
            break;
        }

        sleep(1);
    }

    return NULL;
}

void solveInParallel()
{
    pthread_t solverThreadIDs[MAX_PARALLEL_JOBS];


    pthread_t monitorThreadID;

    HEXAGON_AS_INT hexagonsPerSolver = TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED / parallelJobs;
    HEXAGON_AS_INT hexagonAllocation = 0;

    setbuf(stdout, NULL);

    for(unsigned long i = 0; i < parallelJobs; i++)
    {
        threadConfigs[i].firstHexagon = hexagonAllocation;
        hexagonAllocation += hexagonsPerSolver;

        if(!i)
        {
            hexagonAllocation += TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED % parallelJobs;
        }

        threadConfigs[i].lastHexagon = hexagonAllocation;

        if(pthread_create(&solverThreadIDs[i], NULL, solverThread, &threadConfigs[i]))
        {
            printf("Failed to create enough solver threads.\r\n");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_create(&monitorThreadID, NULL, monitorThread, NULL))
    {
        printf("Failed to create the monitor thread.\r\n");
        exit(EXIT_FAILURE);
    }

    for(unsigned long i = 0; i < parallelJobs; i++)
    {
        pthread_join(solverThreadIDs[i], NULL);
    }

    pthread_join(monitorThreadID, NULL);

    printf("Tried all possible hexagons.\r\n");
}

int main(int argc, char ** argv)
{
    opterr = true;

    int option;

    while((option = getopt(argc, argv, "hj:p")) != -1)
    {
        switch(option)
        {
            case 'h':
                printf("Usage: hexxer [options]\r\n");
                printf("  -h  Display this help.\r\n");
                printf("  -j  Number of parallel jobs to run. (Runs in serial mode if unspecified.)\r\n");
                printf("  -p  Render and print all discovered solutions.\r\n");
                return EXIT_SUCCESS;

            case 'j':
                parallelJobs = strtoul(optarg, NULL, 10);
                if(!parallelJobs)
                {
                    printf("Invalid number of parallel jobs.\r\n");
                    return EXIT_FAILURE;
                }
                else if(parallelJobs > MAX_PARALLEL_JOBS)
                {
                    printf("Parallel jobs limited to %i.\r\n", MAX_PARALLEL_JOBS);
                    return EXIT_FAILURE;
                }
                break;

            case 'p':
                printHexagons = true;
                break;

            case '?':
            default:
                return EXIT_FAILURE;
        }
    }

    if(parallelJobs)
    {
        solveInParallel();
    }
    else
    {
        solveInSerial();
    }

    return EXIT_SUCCESS;
}
