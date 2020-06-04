#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "analisys.h"

Analysis initAnalysis()
{
    Analysis a;
    a.val = 0;
    return a;
}

void printAnalysis(Analysis a)
{
    fprintf(stdout, "%d", a.val);
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    a.val = 0;
    return a;
}