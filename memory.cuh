#ifndef HEXXER_MEMORY_H
#define HEXXER_MEMORY_H

#include "hexagon.cuh"

extern "C" HEXAGON_AS_INT solutionsStored;

extern "C" void memoryInit();
extern "C" void storeSolution(HEXAGON_AS_INT solution);
extern "C" HEXAGON_AS_INT retrieveSolution(HEXAGON_AS_INT solutionID);

#endif //HEXXER_MEMORY_H