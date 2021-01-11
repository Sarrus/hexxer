//
// Created by Michael Bell on 11/01/2021.
//

#ifndef HEXXER_MEMORY_H
#define HEXXER_MEMORY_H

#include "hexagon.h"

HEXAGON_AS_INT solutionsStored;

void memoryInit();
void storeSolution(HEXAGON_AS_INT solution);
HEXAGON_AS_INT retrieveSolution(HEXAGON_AS_INT solutionID);

#endif //HEXXER_MEMORY_H
