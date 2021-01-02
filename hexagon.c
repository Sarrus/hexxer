//
// Created by Michael Bell on 02/01/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include "hexagon.h"

char colourToChar(COLOUR colour)
{
    switch(colour)
    {

        case RED:
            return 'R';
        case YELLOW:
            return 'Y';
        case GREEN:
            return 'G';
        case BLUE:
            return 'B';
        default:
            exit(EXIT_FAILURE);
    }
}

void printHexagon(HEXAGON * hexagon)
{
    const char hexMap[17][29] = {
            {"      /\\  /\\  /\\\r\n"},         //0
            {"     /  \\/  \\/  \\\r\n"},        //1
            {"    | %c | %c | %c |\r\n"},           //2
            {"    /\\  /\\  /\\  /\\\r\n"},      //3
            {"   /  \\/  \\/  \\/  \\\r\n"},     //4
            {"  | %c | %c | %c | %c |\r\n"},         //5
            {"  /\\  /\\  /\\  /\\  /\\\r\n"},   //6
            {" /  \\/  \\/  \\/  \\/  \\\r\n"},  //7
            {"| %c | %c | %c | %c | %c |\r\n"},       //8
            {" \\  /\\  /\\  /\\  /\\  /\r\n"},  //9
            {"  \\/  \\/  \\/  \\/  \\/\r\n"},   //10
            {"  | %c | %c | %c | %c |\r\n"},         //11
            {"   \\  /\\  /\\  /\\  /\r\n"},     //12
            {"    \\/  \\/  \\/  \\/\r\n"},      //13
            {"    | %c | %c | %c |\r\n"},           //14
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
                        colourToChar(hexagon->row0[0]),
                        colourToChar(hexagon->row0[1]),
                        colourToChar(hexagon->row0[2])
                        );
                break;

            case 5:
                printf(
                        hexMap[i],
                        colourToChar(hexagon->row1[0]),
                        colourToChar(hexagon->row1[1]),
                        colourToChar(hexagon->row1[2]),
                        colourToChar(hexagon->row1[3])
                );
                break;

            case 8:
                printf(
                        hexMap[i],
                        colourToChar(hexagon->row2[0]),
                        colourToChar(hexagon->row2[1]),
                        colourToChar(hexagon->row2[2]),
                        colourToChar(hexagon->row2[3]),
                        colourToChar(hexagon->row2[4])
                );
                break;

            case 11:
                printf(
                        hexMap[i],
                        colourToChar(hexagon->row3[0]),
                        colourToChar(hexagon->row3[1]),
                        colourToChar(hexagon->row3[2]),
                        colourToChar(hexagon->row3[3])
                );
                break;

            case 14:
                printf(
                        hexMap[i],
                        colourToChar(hexagon->row4[0]),
                        colourToChar(hexagon->row4[1]),
                        colourToChar(hexagon->row4[2])
                );
                break;

            default:
                printf(hexMap[i]);
        }
    }
}