#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "analisys.h"

Analysis initAnalysis()
{
    Analysis a;

    int i;
    for(i = 0; i < 9; i++)
    {
        a.values[i] = 0;
    }

    return a;
}

void printAnalysis(const int file, Analysis a)
{
    write(file, a.values, sizeof(a.values));
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    read(file, a.values, sizeof(a.values));
    return a;
}

void addCharAnalysis(Analysis a, char c)
{
    if(isText(c))
    {
        if(isUppLetter(c))
            a.values[AN_UPPLT]++;
        else if(isLowLetter(c))
            a.values[AN_LOWLT]++;
        else if(isNumber(c))
            a.values[AN_NUMBR]++;
        else if(isMathSymbol(c))
            a.values[AN_MATHS]++;
        else if(isPunctuation(c))
            a.values[AN_PUNCT]++;
        else if(isBracket(c))
            a.values[AN_BRCKT]++;
        else if(isSpace(c))
            a.values[AN_SPACE]++;
        else
            a.values[AN_OTEXT]++;        
    }
    else
    {
        a.values[AN_OTHER]++;
    }
    
}

int isText(char c)         { return (c >= ' ' && c <= '~')              ? 1 : 0; }
int isUppLetter(char c)    { return (c >= 'A' && c <= 'Z')              ? 1 : 0; }
int isLowLetter(char c)    { return (c >= 'a' && c <= 'z')              ? 1 : 0; }
int isNumber(char c)       { return (c >= '0' && c <= '9')              ? 1 : 0; }
int isMathSymbol(char c)   { return (strstr("=<>+-*/", &c) != NULL)     ? 1 : 0; }
int isPunctuation(char c)  { return (strstr(".,:;'!?`\"", &c) != NULL)  ? 1 : 0; }
int isBracket(char c)      { return (strstr("()[]{}", &c) != NULL)      ? 1 : 0; }
int isSpace(char c)        { return (c == ' ')                          ? 1 : 0; }