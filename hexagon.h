//
// Created by Michael Bell on 02/01/2021.
//

#ifndef HEXXER_HEXAGON_H
#define HEXXER_HEXAGON_H

#define TOTAL_HEXAGONS_WITH_LFET_RED_LOCKED 0x1000000000

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

#endif //HEXXER_HEXAGON_H
