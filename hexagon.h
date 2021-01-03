//
// Created by Michael Bell on 02/01/2021.
//

#ifndef HEXXER_HEXAGON_H
#define HEXXER_HEXAGON_H

#define TOTAL_HEXAGONS_WITH_LEFT_RED_LOCKED 0x1000000000
#define TOTAL_SEGMENTS_WITH_LEFT_RED_LOCKED 18
#define TOTAL_SEGMENTS 19

enum colours{
    RED,
    YELLOW,
    GREEN,
    BLUE
};

#define COLOUR enum colours

struct hexagon{
    COLOUR row0[3];
    COLOUR row1[4];
    COLOUR row2[5];
    COLOUR row3[4];
    COLOUR row4[3];
};

#define HEXAGON struct hexagon

void printHexagon(HEXAGON * hexagon);
void longToHexagon(unsigned long number, HEXAGON * hexagon, bool lockLeftRed);
bool validateSolution(unsigned long solution);

#endif //HEXXER_HEXAGON_H