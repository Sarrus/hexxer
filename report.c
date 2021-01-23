//
// Created by Michael Bell on 13/01/2021.
//

#include "report.h"
#include "hexagon.h"
#include "memory.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define FP int

FP reportHandle;

const char * reportStart = "<!DOCTYPE html>\n"
                           "<html lang=\"en\">\n"
                           "<head>\n"
                           "    <meta charset=\"UTF-8\">\n"
                           "    <title>Hex Map</title>\n"
                           "    <style>\n"
                           "        .hex {\n"
                           "            float: left;\n"
                           "            margin-left: 3px;\n"
                           "            margin-bottom: -26px;\n"
                           "        }\n"
                           "        .hex .top {\n"
                           "            width: 0;\n"
                           "            border-bottom: 30px solid #6C6;\n"
                           "            border-left: 52px solid transparent;\n"
                           "            border-right: 52px solid transparent;\n"
                           "        }\n"
                           "        .hex .middle {\n"
                           "            width: 104px;\n"
                           "            height: 60px;\n"
                           "            background: #6C6;\n"
                           "        }\n"
                           "        .hex .bottom {\n"
                           "            width: 0;\n"
                           "            border-top: 30px solid #6C6;\n"
                           "            border-left: 52px solid transparent;\n"
                           "            border-right: 52px solid transparent;\n"
                           "        }\n"
                           "        .hex-row {\n"
                           "            clear: left;\n"
                           "        }\n"
                           "        .hex-row.even {\n"
                           "            margin-left: 53px;\n"
                           "        }\n"
                           "    </style>\n"
                           "</head>\n"
                           "<body>\n";

const char * oddRowStart = "    <div class=\"hex-row\">\n";

const char * evenRowStart = "    <div class=\"hex-row even\">\n";

const char * rowEnd = "    </div>\n";

const char * emptyHexagon = "        <div class=\"hex\">\n"
                            "            <div class=\"top\" style=\"border-bottom: 30px solid transparent;\"></div>\n"
                            "            <div class=\"middle\" style=\"background: transparent;\"></div>\n"
                            "            <div class=\"bottom\" style=\"border-top: 30px solid transparent;\"></div>\n"
                            "        </div>\n";

const char * colouredHexagons[4] = {
        [RED] =    "        <div class=\"hex\">\n"
                   "            <div class=\"top\" style=\"border-bottom: 30px solid red;\"></div>\n"
                   "            <div class=\"middle\" style=\"background: red;\"></div>\n"
                   "            <div class=\"bottom\" style=\"border-top: 30px solid red;\"></div>\n"
                   "        </div>\n",
        [YELLOW] = "        <div class=\"hex\">\n"
                   "            <div class=\"top\" style=\"border-bottom: 30px solid yellow;\"></div>\n"
                   "            <div class=\"middle\" style=\"background: yellow;\"></div>\n"
                   "            <div class=\"bottom\" style=\"border-top: 30px solid yellow;\"></div>\n"
                   "        </div>\n",
        [GREEN] =  "        <div class=\"hex\">\n"
                   "            <div class=\"top\" style=\"border-bottom: 30px solid green;\"></div>\n"
                   "            <div class=\"middle\" style=\"background: green;\"></div>\n"
                   "            <div class=\"bottom\" style=\"border-top: 30px solid green;\"></div>\n"
                   "        </div>\n",
        [BLUE] =   "        <div class=\"hex\">\n"
                   "            <div class=\"top\" style=\"border-bottom: 30px solid blue;\"></div>\n"
                   "            <div class=\"middle\" style=\"background: blue;\"></div>\n"
                   "            <div class=\"bottom\" style=\"border-top: 30px solid blue;\"></div>\n"
                   "        </div>\n"
};

const char * reportEnd = "</body>\n"
                         "</html>\n";

const char * breakBreak ="    <br>\n"
                         "    <br>\n"
                         "    <br>\n"
                         "    <br>\n"
                         "    <br>\n"
                         "    <br>\n";

bool openReport(const char * location)
{
    if((reportHandle = open(location, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void generateReport()
{
    write(reportHandle, reportStart, strlen(reportStart));

    size_t cellHTMLStrlen[4];

    for(char i = 0; i < 4; i++)
    {
        cellHTMLStrlen[i] = strlen(colouredHexagons[i]);
    }

    size_t evenRowStartStrlen = strlen(evenRowStart);
    size_t oddRowStartStrlen = strlen(oddRowStart);
    size_t rowEndStrlen = strlen(rowEnd);
    size_t emptyHexagonStrlen = strlen(emptyHexagon);
    size_t breakBreakStrlen = strlen(breakBreak);

    HEXAGON renderedHexagon;
    for(HEXAGON_AS_INT i = 0; i < solutionsStored; i++)
    {
        longToHexagon(retrieveSolution(i), &renderedHexagon, false);

        write(reportHandle, evenRowStart, evenRowStartStrlen);
        write(reportHandle, emptyHexagon, emptyHexagonStrlen);
        for(char j = 0; j < 3; j++)
        {
            write(reportHandle, colouredHexagons[renderedHexagon.row0[j]], cellHTMLStrlen[renderedHexagon.row0[j]]);
        }
        write(reportHandle, rowEnd, rowEndStrlen);

        write(reportHandle, oddRowStart, oddRowStartStrlen);
        write(reportHandle, emptyHexagon, emptyHexagonStrlen);
        for(char j = 0; j < 4; j++)
        {
            write(reportHandle, colouredHexagons[renderedHexagon.row1[j]], cellHTMLStrlen[renderedHexagon.row1[j]]);
        }
        write(reportHandle, rowEnd, rowEndStrlen);

        write(reportHandle, evenRowStart, evenRowStartStrlen);
        for(char j = 0; j < 5; j++)
        {
            write(reportHandle, colouredHexagons[renderedHexagon.row2[j]], cellHTMLStrlen[renderedHexagon.row2[j]]);
        }
        write(reportHandle, rowEnd, rowEndStrlen);

        write(reportHandle, oddRowStart, oddRowStartStrlen);
        write(reportHandle, emptyHexagon, emptyHexagonStrlen);
        for(char j = 0; j < 4; j++)
        {
            write(reportHandle, colouredHexagons[renderedHexagon.row3[j]], cellHTMLStrlen[renderedHexagon.row3[j]]);
        }
        write(reportHandle, rowEnd, rowEndStrlen);

        write(reportHandle, evenRowStart, evenRowStartStrlen);
        write(reportHandle, emptyHexagon, emptyHexagonStrlen);
        for(char j = 0; j < 3; j++)
        {
            write(reportHandle, colouredHexagons[renderedHexagon.row4[j]], cellHTMLStrlen[renderedHexagon.row4[j]]);
        }
        write(reportHandle, rowEnd, rowEndStrlen);

        write(reportHandle, breakBreak, breakBreakStrlen);
    }

    write(reportHandle, reportEnd, strlen(reportEnd));
}