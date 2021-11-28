#include <iostream>
#include <unistd.h>
#include "memory.cuh"
#include "hexagon.cuh"
#include "configuration.cuh"

#define THREAD_COUNT 80 * 256
#define FOUND_SOLUTION_POOL_SIZE 10000

__device__
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

__device__ HEXAGON_AS_INT threadFoundSolutions[THREAD_COUNT];
__device__ HEXAGON_AS_INT threadTriedSolutions[THREAD_COUNT];
__device__ HEXAGON_AS_INT threadFoundSolutionPool[THREAD_COUNT][FOUND_SOLUTION_POOL_SIZE];
__device__ HEXAGON_AS_INT threadFoundSolutionsLastPulledTo[THREAD_COUNT];

__device__
bool validateSolution(HEXAGON_AS_INT solution)
{
    char reds = 0;
    char yellows = 0;
    char greens = 0;
    char blues = 0;

    char redsAt[TOTAL_SEGMENTS];
    char yellowsAt[TOTAL_SEGMENTS];
    char greensAt[TOTAL_SEGMENTS];
    char bluesAt[TOTAL_SEGMENTS];

    // Count the colours used and record their locations. We don't count the last cell yet because it is not included
    // in the first validation stage.
    for(char i = 0; i < TOTAL_SEGMENTS_WITH_LEFT_RED_LOCKED; i++)
    {
        switch((solution >> i * 2) % 4)
        {
            case RED:
                redsAt[reds] = i;
                reds++;
                break;

            case YELLOW:
                yellowsAt[yellows] = i;
                yellows++;
                break;

            case GREEN:
                greensAt[greens] = i;
                greens++;
                break;

            case BLUE:
                bluesAt[blues] = i;
                blues++;
                break;
        }
    }

    if(
            (reds < 3)
            || (yellows < 3)
            || (greens < 3)
            || (blues < 3)
            )
    {
        // Less than three of a colour found
        return false;
    }

    // Now we add the last cell to the tally.
    redsAt[reds] = LOCKED_RED_LOCATION;
    reds++;

    for(char i = 0; i < greens; i++)
    {
        char redsAroundGreen = 0;

        for(char j = 0; j < cellRelationships[greensAt[i]].countOfRelationships; j++)
        {
            for(char k = 0; k < reds; k++)
            {
                if(redsAt[k] == cellRelationships[greensAt[i]].relatedCells[j])
                {
                    redsAroundGreen++;
                }
            }
        }

        if(redsAroundGreen != 3)
        {
            // green not surrounded by three reds found
            return false;
        }
    }

    for(char i = 0; i < blues; i++)
    {
        char yellowsAroundBlue = 0;

        for(char j = 0; j < cellRelationships[bluesAt[i]].countOfRelationships; j++)
        {
            for(char k = 0; k < yellows; k++)
            {
                if(yellowsAt[k] == cellRelationships[bluesAt[i]].relatedCells[j])
                {
                    yellowsAroundBlue++;
                }
            }
        }

        if(yellowsAroundBlue != 2)
        {
            // Blue not surrounded by two yellows found
            return false;
        }
    }

    for(char i = 0; i < yellows; i++)
    {
        char coloursFoundSurrounding = 0b000; // 0b001 = red, 0b010 = green, 0b100 = blue

        for(char j = 0; j < cellRelationships[yellowsAt[i]].countOfRelationships; j++)
        {
            for(char k = 0; k < reds; k++)
            {
                if(redsAt[k] == cellRelationships[yellowsAt[i]].relatedCells[j])
                {
                    coloursFoundSurrounding |= 0b001;
                    k = reds;
                }
            }

            for(char k = 0; k < greens; k++)
            {
                if(greensAt[k] == cellRelationships[yellowsAt[i]].relatedCells[j])
                {
                    coloursFoundSurrounding |= 0b010;
                    k = greens;
                }
            }

            for(char k = 0; k < blues; k++)
            {
                if(bluesAt[k] == cellRelationships[yellowsAt[i]].relatedCells[j])
                {
                    coloursFoundSurrounding |= 0b100;
                    k = blues;
                }
            }
        }

        if(coloursFoundSurrounding != 0b111)
        {
            // Yellow not surrounded by at least one of every other colour found
            return false;
        }
    }

    return true;
}

__global__
void prepare()
{
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        threadFoundSolutions[i] = 0;
        threadTriedSolutions[i] = 0;
        threadFoundSolutionsLastPulledTo[i] = 0;
    }
}

__global__
void solver(HEXAGON_AS_INT * aValidSolution, const bool * kernelStop)
{
    HEXAGON_AS_INT start = blockIdx.x * blockDim.x + threadIdx.x;
    HEXAGON_AS_INT step = blockDim.x * gridDim.x;

    for(HEXAGON_AS_INT i = start; i < TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED; i += step)
    {
        if(validateSolution(i))
        {
            //printf("%lu\r\n", i);
            threadFoundSolutionPool[start][threadFoundSolutions[start]] = i;
            threadFoundSolutions[start]++;
            *aValidSolution = i;
        }
        threadTriedSolutions[start]++;

        if(*kernelStop)
        {
            return;
        }
    }
}

__global__
void retrieveResult(HEXAGON_AS_INT * solutionCount)
{
    *solutionCount = 0;
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        *solutionCount += threadFoundSolutions[i];
    }
}

__global__
void manageProgress(HEXAGON_AS_INT * triedSolutions, HEXAGON_AS_INT * foundSolutions, HEXAGON_AS_INT * solutionCount)
{
    *triedSolutions = 0;
    for(HEXAGON_AS_INT i = 0; i < THREAD_COUNT; i++)
    {
        *triedSolutions += threadTriedSolutions[i];
        while(threadFoundSolutionsLastPulledTo[i] < threadFoundSolutions[i])
        {
            foundSolutions[*solutionCount] = threadFoundSolutionPool[i][threadFoundSolutionsLastPulledTo[i]];
            (*solutionCount)++;
            threadFoundSolutionsLastPulledTo[i]++;
        }
    }
}

extern "C" void solveWithCUDA()
{
    HEXAGON_AS_INT * solutionCount;
    HEXAGON_AS_INT * aValidSolution;
    HEXAGON_AS_INT * triedSolutions;
    HEXAGON_AS_INT * foundSolutions;
    bool * kernelStop;
    cudaMallocManaged(&solutionCount, sizeof(HEXAGON_AS_INT));
    cudaMallocManaged(&aValidSolution, sizeof(HEXAGON_AS_INT));
    cudaMallocManaged(&triedSolutions, sizeof(HEXAGON_AS_INT));
    cudaMallocManaged(&foundSolutions, sizeof(HEXAGON_AS_INT) * FOUND_SOLUTION_POOL_SIZE);
    cudaMallocManaged(&kernelStop, sizeof(bool));

    char solutionString[ID_PRINT_MAX_SIZE];

    cudaStream_t mainStream, verificationStream;
    cudaStreamCreate(&mainStream);
    cudaStreamCreate(&verificationStream);

    *kernelStop = false;

    prepare<<<1, 1, 0, mainStream>>>();
    solver<<<80, 256, 0, mainStream>>>(aValidSolution, kernelStop);
    //retrieveResult<<<1, 1, 0, mainStream>>>(solutionCount);

    HEXAGON_AS_INT solutionsPushed = 0;
    HEXAGON_AS_INT uniqueSolutions = 0;

    bool looping = true;
    bool lastPrintWasProgressLine = false;

    while(looping)
    {
        if(cudaStreamQuery(mainStream) == cudaSuccess)
        {
            looping = false;
        }
        manageProgress<<<1, 1, 0, verificationStream>>>(triedSolutions, foundSolutions, solutionCount);
        cudaStreamSynchronize(verificationStream);
        while(solutionsPushed < *solutionCount)
        {
            if(lastPrintWasProgressLine)
            {
                fprintf(stderr, "%c[2K\r", 27);
                lastPrintWasProgressLine = false;
            }
            fprintf(stderr, "Solution found at hexagon no. %lu, ", foundSolutions[solutionsPushed]);

            if(saveAllSolutionIDs)
            {
                snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", foundSolutions[solutionsPushed]);
                write(allSolutionsLocationHandle, solutionString, strlen(solutionString));
            }

            HEXAGON_AS_INT matchedSolution = checkSolutionForVisualMatches(foundSolutions[solutionsPushed]);
            if(stopOnFirstSolution)
            {
                fprintf(stderr, "stopping.\r\n");
                *kernelStop = true;
                looping = false;
                break;
            }
            else if(matchedSolution)
            {
                fprintf(stderr, "visually matches solution no. %lu ", matchedSolution);
            }
            else
            {
                uniqueSolutions++;
                fprintf(stderr, "no visual matches found. ");
                if(saveUniqueSolutionIDs)
                {
                    snprintf(solutionString, ID_PRINT_MAX_SIZE, "%lu\r\n", foundSolutions[solutionsPushed]);
                    write(uniqueSolutionsLocationHandle, solutionString, strlen(solutionString));
                }
            }
            fprintf(stderr, "%lu solutions found so far, %lu visually unique.\r\n", solutionsPushed, uniqueSolutions);

            storeSolution(foundSolutions[solutionsPushed]);
            solutionsPushed++;
        }

        if(lastPrintWasProgressLine)
        {
            fprintf(stderr, "%c[2K\r", 27);
        }
        fprintf(stderr,
                "%lu hexagons processed so far, %f%% of total.",
                *triedSolutions,
                100 * (float)*triedSolutions / (float)TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED
        );
        lastPrintWasProgressLine = true;

        sleep(1);
    }

    cudaDeviceSynchronize();
    if(stopOnFirstSolution)
    {
        fprintf(stderr, "\r\nStopped.\r\n");
    }
    else
    {
        fprintf(stderr, "\r\nTried all possible solutions.\r\n");
    }
}