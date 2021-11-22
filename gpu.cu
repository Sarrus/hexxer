#include <iostream>

#define HEXAGON_AS_INT u_int64_t
#define TOTAL_SEGMENTS 19
#define TOTAL_SEGMENTS_WITH_LEFT_RED_LOCKED 18
#define LOCKED_RED_LOCATION 18

enum colours{
    RED,
    YELLOW,
    GREEN,
    BLUE
};

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
void solver(bool * result)
{
    *result = validateSolution(152953033);
}

extern "C" void solveWithCUDA()
{
    bool * result;
    cudaMallocManaged(&result, sizeof(bool));
    solver<<<1, 1>>>(result);
    cudaDeviceSynchronize();
    if(*result)
    {
        std::cerr << "Valid Solution" << std::endl;
    }
    else
    {
        std::cerr << "Invalid Solution" << std::endl;
    }
}