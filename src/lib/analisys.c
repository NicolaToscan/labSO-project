#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "analisys.h"

Analysis initAnalysis()
{
    Analysis a;
    
    for(int i = 0; i < 9; i++)
        a.values[i] = 0;

    return a;
}

void printAnalysis(const int file, Analysis a)
{
    char buffer[9 * ULONG_MAXLEN];

    for(int i = 0; i < 9; i++)
    {
        intToStr(a.values[i], buffer, i * ULONG_MAXLEN);
    }

    write(file, buffer, sizeof(buffer));
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    char buffer[9 * ULONG_MAXLEN];

    read(file, buffer, sizeof(buffer));
    
    for(int i = 0; i < 9; i++)
    {
        a.values[i] = (buffer, i * ULONG_MAXLEN, a.values[i]);
    }

    return a;
}

void intToStr(ulong value, char *str, int startIndex)
{
    int i;
    for(i = 0; i < ULONG_MAXLEN; i++)
    {
        str[startIndex + i] = '0';
    }

    int index = ULONG_MAXLEN - 1;
    while(value > 0)
    {
        str[startIndex + index--] = '0' + (value % 10);
        value /= 10;
    }
}

ulong strToInt(char *str, int startIndex)
{
    ulong ul = 0;
    int index = 0;
    char c[1];

    while(index < ULONG_MAXLEN)
    {
        c[startIndex + 0] = str[index++];
        ul = (ul * 10) + atoi(c);
    }

    return ul;
}

// Forse inutile
void intToStr(ulong value, char str[ULONG_MAXLEN])
{
    int i;
    for(i = 0; i < ULONG_MAXLEN; i++)
    {
        str[i] = '0';
    }

    int index = ULONG_MAXLEN - 1;
    while(value > 0)
    {
        str[index--] = '0' + (value % 10);
        value /= 10;
    }
}

// Forse inutile
void strToInt(char str[ULONG_MAXLEN], ulong *value)
{
    ulong ul = 0;
    int index = 0;
    char c[1];

    while(index < ULONG_MAXLEN)
    {
        c[0] = str[index++];
        ul = (ul * 10) + atoi(c);
    }

    *value = ul;
}

bool isText(char c)         { return c >= ' ' && c <= '~'; }
bool isUppLetter(char c)    { return c >= 'A' && c <= 'Z'; }
bool isLowLetter(char c)    { return c >= 'a' && c <= 'z'; }
bool isNumber(char c)       { return c >= '0' && c <= '9'; }
bool isMathSymbol(char c)   { return strstr("=<>+-*/", c) != NULL; }
bool isPunctuation(char c)  { return strstr(".,:;'!?`\"", c) != NULL; }
bool isBracket(char c)      { return strstr("()[]{}", c) != NULL; }
bool isSpace(char c)        { return c == ' '; }