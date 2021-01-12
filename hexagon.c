//
// Created by Michael Bell on 02/01/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hexagon.h"
#include "memory.h"

/*
      /\  /\  /\
     /  \/  \/  \
    | 0 | 1 | 2 |
    /\  /\  /\  /\
   /  \/  \/  \/  \
  | 3 | 4 | 5 | 6 |
  /\  /\  /\  /\  /\
 /  \/  \/  \/  \/  \
| 7 | 8 | 9 |10 |11 |
 \  /\  /\  /\  /\  /
  \/  \/  \/  \/  \/
  |12 |13 |14 |15 |
   \  /\  /\  /\  /
    \/  \/  \/  \/
    |16 |17 |18 |
     \  /\  /\  /
      \/  \/  \/
 */

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

const char cellRotationMap[TOTAL_SEGMENTS] = {
        2,  // 0
        6,  // 1
        11, // 2
        1,  // 3
        5,  // 4
        10, // 5
        15, // 6
        0,  // 7
        4,  // 8
        9,  // 9
        14, // 10
        18, // 11
        3,  // 12
        8,  // 13
        13, // 14
        17, // 15
        7,  // 16
        12, // 17
        16  // 18
};

const char cellFlipMap1[TOTAL_SEGMENTS] = {
        18, // 0
        15, // 1
        11, // 2
        17, // 3
        14, // 4
        10, // 5
        6,  // 6
        16, // 7
        13, // 8
        9,  // 9
        5,  // 10
        2,  // 11
        12, // 12
        8,  // 13
        4,  // 14
        1,  // 15
        7,  // 16
        3,  // 17
        0   // 18
};

const char cellFlipMap2[TOTAL_SEGMENTS] = {
       16, // 0
       17, // 1
       18, // 2
       12, // 3
       13, // 4
       14, // 5
       15, // 6
       7,  // 7
       8,  // 8
       9,  // 9
       10, // 10
       11, // 11
       3,  // 12
       4,  // 13
       5,  // 14
       6,  // 15
       0,  // 16
       1,  // 17
       2   // 18
};

char * colourToString(COLOUR colour)
{
    switch(colour)
    {

        case RED:
            return "\x1B[31mR\x1B[0m";
        case YELLOW:
            return "\x1B[33mY\x1B[0m";
        case GREEN:
            return "\x1B[32mG\x1B[0m";
        case BLUE:
            return "\x1B[34mB\x1B[0m";
        default:
            exit(EXIT_FAILURE);
    }
}

void printHexagon(HEXAGON * hexagon)
{
    const char hexMap[17][29] = {
            {"      /\\  /\\  /\\\r\n"},         //0
            {"     /  \\/  \\/  \\\r\n"},        //1
            {"    | %s | %s | %s |\r\n"},           //2
            {"    /\\  /\\  /\\  /\\\r\n"},      //3
            {"   /  \\/  \\/  \\/  \\\r\n"},     //4
            {"  | %s | %s | %s | %s |\r\n"},         //5
            {"  /\\  /\\  /\\  /\\  /\\\r\n"},   //6
            {" /  \\/  \\/  \\/  \\/  \\\r\n"},  //7
            {"| %s | %s | %s | %s | %s |\r\n"},       //8
            {" \\  /\\  /\\  /\\  /\\  /\r\n"},  //9
            {"  \\/  \\/  \\/  \\/  \\/\r\n"},   //10
            {"  | %s | %s | %s | %s |\r\n"},         //11
            {"   \\  /\\  /\\  /\\  /\r\n"},     //12
            {"    \\/  \\/  \\/  \\/\r\n"},      //13
            {"    | %s | %s | %s |\r\n"},           //14
            {"     \\  /\\  /\\  /\r\n"},        //15
            {"      \\/  \\/  \\/\r\n"}          //16
    };

    for (int i = 0; i < 17; i++)
    {
        switch(i)
        {
            case 2:
                printf(
                        hexMap[i],
                        colourToString(hexagon->row0[0]),
                        colourToString(hexagon->row0[1]),
                        colourToString(hexagon->row0[2])
                        );
                break;

            case 5:
                printf(
                        hexMap[i],
                        colourToString(hexagon->row1[0]),
                        colourToString(hexagon->row1[1]),
                        colourToString(hexagon->row1[2]),
                        colourToString(hexagon->row1[3])
                );
                break;

            case 8:
                printf(
                        hexMap[i],
                        colourToString(hexagon->row2[0]),
                        colourToString(hexagon->row2[1]),
                        colourToString(hexagon->row2[2]),
                        colourToString(hexagon->row2[3]),
                        colourToString(hexagon->row2[4])
                );
                break;

            case 11:
                printf(
                        hexMap[i],
                        colourToString(hexagon->row3[0]),
                        colourToString(hexagon->row3[1]),
                        colourToString(hexagon->row3[2]),
                        colourToString(hexagon->row3[3])
                );
                break;

            case 14:
                printf(
                        hexMap[i],
                        colourToString(hexagon->row4[0]),
                        colourToString(hexagon->row4[1]),
                        colourToString(hexagon->row4[2])
                );
                break;

            default:
                printf(hexMap[i]);
        }
    }
}

void longToHexagon(HEXAGON_AS_INT number, HEXAGON * hexagon, bool lockLeftRed)
{
    for(int i = 0; i < 3; i++)
    {
        hexagon->row0[i] = (number >> (i * 2)) % 4;
        hexagon->row4[i] = (number >> ((i + 16) * 2)) % 4;
    }

    for(int i = 0; i < 4; i++)
    {
        hexagon->row1[i] = (number >> ((i + 3) * 2)) % 4;
        hexagon->row3[i] = (number >> ((i + 12) * 2)) % 4;
    }

    for(int i = 0; i < 5; i++)
    {
        hexagon->row2[i] = (number >> ((i + 7) * 2)) % 4;
    }
}

HEXAGON_AS_INT rearrangeHexagon(HEXAGON_AS_INT hexagon, const char * substitutionMap)
{
    HEXAGON_AS_INT newHexagon = 0;

    for(char i = 0; i < TOTAL_SEGMENTS; i++)
    {
        HEXAGON_AS_INT mask = 0b11;
        mask <<= (i * 2);
        mask &= hexagon;
        mask >>= (i * 2);
        mask <<= substitutionMap[i] * 2;
        newHexagon |= mask;
    }

    return newHexagon;
}

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

HEXAGON_AS_INT checkSolutionForVisualMatches(HEXAGON_AS_INT solution)
{
    for(HEXAGON_AS_INT i = 0; i < solutionsStored; i++)
    {
        // Check for rotational matches
        HEXAGON_AS_INT rotatedSolution = solution;
        HEXAGON_AS_INT solutionToTestAgainst = retrieveSolution(i);
        for(char j = 0; j < 5; j++)
        {
            rotatedSolution = rearrangeHexagon(rotatedSolution, cellRotationMap);
            if(solutionToTestAgainst == rotatedSolution)
            {
                return solutionToTestAgainst;
            }
        }

        // Check for rotational matches after flipping between edges
        rotatedSolution = rearrangeHexagon(solution, cellFlipMap1);

        for(char j = 0; j < 6; j++)
        {
            if(solutionToTestAgainst == rotatedSolution)
            {
                return solutionToTestAgainst;
            }
            rotatedSolution = rearrangeHexagon(rotatedSolution, cellRotationMap);
        }

        // Check for rotational matches after flipping between corners
        rotatedSolution = rearrangeHexagon(solution, cellFlipMap2);

        for(char j = 0; j < 6; j++)
        {
            if(solutionToTestAgainst == rotatedSolution)
            {
                return solutionToTestAgainst;
            }
            rotatedSolution = rearrangeHexagon(rotatedSolution, cellRotationMap);
        }
    }

    return 0;
}