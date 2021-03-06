#ifndef HEXXER_CONFIGURATION_H
#define HEXXER_CONFIGURATION_H

#define FP int
#define ID_PRINT_MAX_SIZE 15

extern "C" bool printHexagons;
extern "C" bool printVisualMatches;
extern "C" bool saveHTMLReport;
extern "C" bool stopOnFirstSolution;
extern "C" bool saveAllSolutionIDs;
extern "C" bool saveUniqueSolutionIDs;

extern "C" FP allSolutionsLocationHandle;
extern "C" FP uniqueSolutionsLocationHandle;

#endif //HEXXER_CONFIGURATION_H