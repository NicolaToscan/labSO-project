#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "analisys.h"
#include "filemanager.h"

Analysis initAnalysis()
{
    Analysis a;

    int i;
    for (i = 0; i < ANAL_LENGTH; i++)
    {
        a.values[i] = 0;
    }

    return a;
}

Analysis analyseFile(char *fileName, int mySection, int totSections)
{
    FILE *file;
    file = fopen(fileName, "r");

    // ATTENZIONE POSSIBILE CASINO
    // FORSE VA RIDOTTA LA LENGTH PER TOGLIERE IL CHAR EOF

    fseek(file, 0, SEEK_END);
    int fileLength = ftell(file);
    pair part = getFileRange(fileLength, mySection, totSections);
    int howmany = part.second - part.first + 1;

    fseek(file, 0, part.first);
    Analysis a = initAnalysis();
    int i = 0;

    while (i < howmany)
    {
        addCharAnalysis(&a, (char)fgetc(file));
        i++;
    }
    fclose(file);

    return a;
}

void printAnalysis(const int file, Analysis a)
{
    write(file, a.values, sizeof(a.values));
    write(file, "\n", 1);
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    read(file, a.values, sizeof(a.values));

    char aCapo;
    read(file, &aCapo, 1);

    return a;
}

void sumAnalysis(Analysis *res, Analysis a)
{
    int i;
    for (i = 0; i < ANAL_LENGTH; i++)
        res->values[i] += a.values[i];
}

void addCharAnalysis(Analysis *a, char c)
{
    if (isText(c))
    {
        if (isUppLetter(c))
            a->values[AN_UPPLT]++;
        else if (isLowLetter(c))
            a->values[AN_LOWLT]++;
        else if (isNumber(c))
            a->values[AN_NUMBR]++;
        else if (isMathSymbol(c))
            a->values[AN_MATHS]++;
        else if (isPunctuation(c))
            a->values[AN_PUNCT]++;
        else if (isBracket(c))
            a->values[AN_BRCKT]++;
        else if (isSpace(c))
            a->values[AN_SPACE]++;
        else
            a->values[AN_OTEXT]++;
    }
    else
    {
        a->values[AN_OTHER]++;
    }
}

void printAnalysisReadable(Analysis a)
{
    fprintf(stderr, "%d - %d - %d - %d - %d - %d - %d - %d - %d\n", (int)a.values[0], (int)a.values[1], (int)a.values[2], (int)a.values[3], (int)a.values[4], (int)a.values[5], (int)a.values[6], (int)a.values[7], (int)a.values[8]);
}

int isText(char c) { return (c >= ' ' && c <= '~') ? 1 : 0; }
int isUppLetter(char c) { return (c >= 'A' && c <= 'Z') ? 1 : 0; }
int isLowLetter(char c) { return (c >= 'a' && c <= 'z') ? 1 : 0; }
int isNumber(char c) { return (c >= '0' && c <= '9') ? 1 : 0; }
int isMathSymbol(char c) { return (strstr("=<>+-*/", &c) != NULL) ? 1 : 0; }
int isPunctuation(char c) { return (strstr(".,:;'!?`\"", &c) != NULL) ? 1 : 0; }
int isBracket(char c) { return (strstr("()[]{}", &c) != NULL) ? 1 : 0; }
int isSpace(char c) { return (c == ' ') ? 1 : 0; }
