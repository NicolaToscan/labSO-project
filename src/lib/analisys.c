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

void printAnalysis(const int file, Analysis a)
{
    write(file, a.val, sizeof(int));
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    read(file, a.val, sizeof(int));
    return a;
}