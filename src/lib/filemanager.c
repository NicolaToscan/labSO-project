#include <stdlib.h>
#include <stdio.h>
#include "analisys.h"
#include "common.h"
#include "filemanager.h"

pair getFileRange(uint32 fileLength, int part, int totParts)
{
    int frazione = fileLength / totParts;
    int resto = fileLength % totParts;
    int firstPartLen = frazione + resto;

    pair p;

    if(part == 1)
    {
        p.first = 0;
        p.second = firstPartLen - 1;
    }
    else
    {
        p.first = firstPartLen + (part - 2) * frazione;
        p.second = p.first + frazione - 1;
    }

    return p;
}