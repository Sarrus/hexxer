#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "hexagon.h"
#include "memory.h"
#include "report.h"

#define FP int

#define MAX_PARALLEL_JOBS 256
#define PARALLEL_SOLVER_ALLOCATION_BLOCK 10000000
#define ID_PRINT_MAX_SIZE 15

bool printHexagons = false;
bool printVisualMatches = false;
unsigned long parallelJobs = 0;
bool saveHTMLReport = false;
bool stopOnFirstSolution = false;
bool saveAllSolutionIDs = false;
bool saveUniqueSolutionIDs = false;

FP allSolutionsLocationHandle;
FP uniqueSolutionsLocationHandle;

struct solverThreadConfig
{
    HEXAGON_AS_INT firstHexagon;
    HEXAGON_AS_INT currentHexagon;
    HEXAGON_AS_INT lastHexagon;
};

#define SOLVER_THREAD_CONFIG struct solverThreadConfig
SOLVER_THREAD_CONFIG threadConfigs[MAX_PARALLEL_JOBS];

HEXAGON_AS_INT solutionsFound = 0;

HEXAGON_AS_INT solverHexagonAllocationQueue = 0;

bool lastPrintWasProgressLine = false;

pthread_mutex_t solutionValidationMutex;

pthread_mutex_t solverAllocationMutex;

pthread_t solverThreadIDs[MAX_PARALLEL_JOBS];

pthread_t monitorThreadID;

pthread_t killerThread;

void solveInSerial()
{

    HEXAGON renderedHexagon;
    HEXAGON_AS_INT matchedSolution;
    char solutionString[ID_PRINT_MAX_SIZE];

    for(HEXAGON_AS_INT i = 0; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i++)
    {
        if(validateSolution(i))
        {
            solutionsFound++;
            fprintf(stderr, "Solution found at hexagon no. %lu, ", i);

            if(saveAllSolutionIDs)
            {
                snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", i);
                write(allSolutionsLocationHandle, solutionString, strlen(solutionString));
            }

            if(stopOnFirstSolution)
            {
                fprintf(stderr, "stopping.\r\n");
                storeSolution(i);
                return;
            }
            else if((matchedSolution = checkSolutionForVisualMatches(i)))
            {
                fprintf(stderr, "visually matches solution no. %lu ", matchedSolution);
            }
            else
            {
                fprintf(stderr, "no visual matches found. ");
                storeSolution(i);
                if(saveUniqueSolutionIDs)
                {
                    snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", i);
                    write(uniqueSolutionsLocationHandle, solutionString, strlen(solutionString));
                }
            }

            fprintf(stderr, "%f%% of all hexagons tried, ", 100 * (float)i / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED);
            fprintf(stderr, "%lu solutions found so far, %lu visually unique.\r\n", solutionsFound, solutionsStored);
            if(printVisualMatches && matchedSolution)
            {
                fprintf(stderr, "Match:\r\n");
                longToHexagon(matchedSolution, &renderedHexagon, false);
                printHexagon(&renderedHexagon);
                fprintf(stderr, "New Solution:\r\n");
                longToHexagon(i, &renderedHexagon, false);
                printHexagon(&renderedHexagon);
            }
            else if(printHexagons)
            {
                longToHexagon(i, &renderedHexagon, false);
                printHexagon(&renderedHexagon);
            }
        }
    }

    fprintf(stderr, "Tried all possible hexagons.\r\n");
}

HEXAGON_AS_INT printParallelProgress()
{
    HEXAGON_AS_INT hexagonsProcessed = solverHexagonAllocationQueue - (PARALLEL_SOLVER_ALLOCATION_BLOCK * parallelJobs);
    for(unsigned long i = 0; i < parallelJobs; i++)
    {
        hexagonsProcessed += threadConfigs[i].currentHexagon - threadConfigs[i].firstHexagon;
    }

    if(lastPrintWasProgressLine)
    {
        fprintf(stderr, "%c[2K\r", 27);
    }

    fprintf(stderr, 
            "%lu hexagons processed so far, %f%% of total.",
            hexagonsProcessed,
            100 * (float)hexagonsProcessed / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED
    );

    lastPrintWasProgressLine = true;

    return hexagonsProcessed;
}

void * stopSolvingInParallel(void * unused)
{
    for(int i = 0; i < parallelJobs; i++)
    {
        pthread_cancel(solverThreadIDs[i]);
    }

    pthread_cancel(monitorThreadID);
    return NULL;
}

void * solverThread(void * config)
{
    SOLVER_THREAD_CONFIG * solverConfig = config;
    HEXAGON renderedHexagon;
    HEXAGON_AS_INT matchedSolution;
    char solutionString[ID_PRINT_MAX_SIZE];

    while(1)
    {
        pthread_mutex_lock(&solverAllocationMutex);

        if(solverHexagonAllocationQueue == TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED)
        {
            pthread_mutex_unlock(&solverAllocationMutex);
            return NULL;
        }

        solverConfig->firstHexagon = solverConfig->currentHexagon = solverHexagonAllocationQueue;

        HEXAGON_AS_INT allocationStep = solverHexagonAllocationQueue + PARALLEL_SOLVER_ALLOCATION_BLOCK;

        if(allocationStep > TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED)
        {
            solverHexagonAllocationQueue = TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED;
        }
        else
        {
            solverConfig->lastHexagon = solverHexagonAllocationQueue = allocationStep;
        }

        pthread_mutex_unlock(&solverAllocationMutex);

        //fprintf(stderr, "This solver's range is: %lu to %lu.\r\n", solverConfig->firstHexagon, solverConfig->lastHexagon);

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
                    fprintf(stderr, "%c[2K\r", 27);
                    //fprintf(stderr, "\r\n");
                    lastPrintWasProgressLine = false;
                }
                fprintf(stderr, "Solution found at hexagon no. %lu, ", solverConfig->currentHexagon);

                if(saveAllSolutionIDs)
                {
                    snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", solverConfig->currentHexagon);
                    write(allSolutionsLocationHandle, solutionString, strlen(solutionString));
                }

                pthread_mutex_lock(&solutionValidationMutex);

                if(stopOnFirstSolution)
                {
                    fprintf(stderr, "stopping.\r\n");
                    storeSolution(solverConfig->currentHexagon);
                    pthread_create(&killerThread, NULL, stopSolvingInParallel, NULL);
                }
                else if((matchedSolution = checkSolutionForVisualMatches(solverConfig->currentHexagon)))
                {
                    fprintf(stderr, "visually matches solution no. %lu ", matchedSolution);
                }
                else
                {
                    fprintf(stderr, "no visual matches found. ");
                    storeSolution(solverConfig->currentHexagon);
                    if(saveUniqueSolutionIDs)
                    {
                        snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", solverConfig->currentHexagon);
                        write(uniqueSolutionsLocationHandle, solutionString, strlen(solutionString));
                    }
                }

                pthread_mutex_unlock(&solutionValidationMutex);

                fprintf(stderr, "%lu solutions found so far, %lu visually unique.\r\n", solutionsFound, solutionsStored);

                if(printVisualMatches && matchedSolution)
                {
                    fprintf(stderr, "Match:\r\n");
                    longToHexagon(matchedSolution, &renderedHexagon, false);
                    printHexagon(&renderedHexagon);
                    fprintf(stderr, "New Solution:\r\n");
                    longToHexagon(solverConfig->currentHexagon, &renderedHexagon, false);
                    printHexagon(&renderedHexagon);
                }
                else if(printHexagons)
                {
                    longToHexagon(solverConfig->currentHexagon, &renderedHexagon, false);
                    printHexagon(&renderedHexagon);
                }

                printParallelProgress();
            }
            else if(stopOnFirstSolution)
            {
                pthread_testcancel();
            }
        }
    }
}

void * monitorThread(void * config)
{
    while(1)
    {
        sleep(1);

        pthread_testcancel();

        if(printParallelProgress() == TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED)
        {
            break;
        }
    }

    return NULL;
}

void solveInParallel()
{
    HEXAGON_AS_INT hexagonsPerSolver = TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED / parallelJobs;
    HEXAGON_AS_INT hexagonAllocation = 0;

    pthread_mutex_init(&solutionValidationMutex, NULL);
    pthread_mutex_init(&solverAllocationMutex, NULL);

    nice(5);

    for(unsigned long i = 0; i < parallelJobs; i++)
    {
//        threadConfigs[i].currentHexagon = threadConfigs[i].firstHexagon = hexagonAllocation;
//        hexagonAllocation += hexagonsPerSolver;
//
//        if(!i)
//        {
//            hexagonAllocation += TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED % parallelJobs;
//        }
//
//        threadConfigs[i].lastHexagon = hexagonAllocation;

//        threadConfigs[i].firstHexagon = 0;
//        threadConfigs[i].currentHexagon = 0;
//        threadConfigs[i].lastHexagon = 0;

        if(pthread_create(&solverThreadIDs[i], NULL, solverThread, &threadConfigs[i]))
        {
            fprintf(stderr, "Failed to create enough solver threads.\r\n");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_create(&monitorThreadID, NULL, monitorThread, NULL))
    {
        fprintf(stderr, "Failed to create the monitor thread.\r\n");
        exit(EXIT_FAILURE);
    }

    for(unsigned long i = 0; i < parallelJobs; i++)
    {
        pthread_join(solverThreadIDs[i], NULL);
    }

    pthread_cancel(monitorThreadID);
    pthread_join(monitorThreadID, NULL);

    if(stopOnFirstSolution)
    {
        fprintf(stderr, " Stopped.\r\n");
    }
    else
    {
        fprintf(stderr, "Tried all possible hexagons.\r\n");
    }
}

int main(int argc, char ** argv)
{
    opterr = true;

    int option;
    HEXAGON displayHexagon;

    while((option = getopt(argc, argv, "d:hj:mo:pr:st:u:")) != -1)
    {
        switch(option)
        {
            case 'd':
                longToHexagon(strtoul(optarg, NULL, 10), &displayHexagon, false);
                printHexagon(&displayHexagon);
                return EXIT_SUCCESS;

            case 'h':
                fprintf(stderr, "Usage: hexxer [options]\r\n");
                fprintf(stderr, "  -d  Display the hexagon specified by its ID then exit.\r\n");
                fprintf(stderr, "  -h  Display this help.\r\n");
                fprintf(stderr, "  -j  Number of parallel jobs to run. (Runs in serial mode if unspecified.)\r\n");
                fprintf(stderr, "  -m  Render and print visual matches.\r\n");
                fprintf(stderr, "  -o  Write the ID of each solution as it is found to the specified location. ('-' to "
                                "write to stdout.)\r\n");
                fprintf(stderr, "  -p  Render and print all discovered solutions.\r\n");
                fprintf(stderr, "  -r  Generate an HTML report of unique solutions saved in the indicated location.\r\n");
                fprintf(stderr, "  -s  Stop when a solution is found.\r\n");
                fprintf(stderr, "  -t  Test a single solution specified by its ID to see if it is considered valid.\r\n");
                fprintf(stderr, "  -u  Write the ID of each visually unique solution as it is found to the specified "
                                "location. ('-' to write to stdout.)\r\n");
                return EXIT_SUCCESS;

            case 'j':
                parallelJobs = strtoul(optarg, NULL, 10);
                if(!parallelJobs)
                {
                    fprintf(stderr, "Invalid number of parallel jobs.\r\n");
                    return EXIT_FAILURE;
                }
                else if(parallelJobs > MAX_PARALLEL_JOBS)
                {
                    fprintf(stderr, "Parallel jobs limited to %i.\r\n", MAX_PARALLEL_JOBS);
                    return EXIT_FAILURE;
                }
                break;

            case 'm':
                printVisualMatches = true;
                break;

            case 'o':
                saveAllSolutionIDs = true;
                if(!strcmp(optarg, "-"))
                {
                    allSolutionsLocationHandle = STDOUT_FILENO;
                }
                else if((allSolutionsLocationHandle = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
                {
                    fprintf(stderr, "Unable to open all solutions file for writing. Error:%i\r\n", errno);
                    return EXIT_FAILURE;
                }
                break;

            case 'p':
                printHexagons = true;
                break;

            case 'r':
                saveHTMLReport = true;
                if(!openReport(optarg))
                {
                    fprintf(stderr, "Unable to open report file for writing. Error:%i\r\n", errno);
                    return EXIT_FAILURE;
                }
                break;

            case 's':
                stopOnFirstSolution = true;
                break;

            case 't':
                fprintf(stderr, "Solution %s is ", optarg);
                if(validateSolution(strtoul(optarg, NULL, 10)))
                {
                    fprintf(stderr, "\x1B[32mVALID\x1B[0m.\r\n");
                    return EXIT_SUCCESS;
                }
                else
                {
                    fprintf(stderr, "\x1B[31mINVALID\x1B[0m.\r\n");
                    return EXIT_FAILURE;
                }

            case 'u':
                saveUniqueSolutionIDs = true;
                if(!strcmp(optarg, "-"))
                {
                    uniqueSolutionsLocationHandle = STDOUT_FILENO;
                }
                else if((uniqueSolutionsLocationHandle = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
                {
                    fprintf(stderr, "Unable to open unique solutions file for writing. Error:%i\r\n", errno);
                    return EXIT_FAILURE;
                }
                break;

            case '?':
            default:
                return EXIT_FAILURE;
        }
    }

    memoryInit();

    if(parallelJobs)
    {
        solveInParallel();
    }
    else
    {
        solveInSerial();
    }

    if(saveHTMLReport)
    {
        generateReport();
    }

    return EXIT_SUCCESS;
}
