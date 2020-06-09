#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "analisys.h"

Analysis initAnalysis()
{
    Analysis a;
    a.valid = true;
    int i;
    for (i = 0; i < ANAL_LENGTH; i++)
    {
        a.values[i] = 0;
    }

    return a;
}

void getFileRange(int len, int i, int n, int *start, int *toRead)
{
    int section = len / n;
    *start = (i * section);
    *toRead = section;
    if (i + 1 == n)
        *toRead += len % n;
}

Analysis analyseFile(char *fileName, int mySection, int totSections)
{
    int fd = open(fileName, O_RDONLY);

    Analysis a = initAnalysis();

    if (fd <= 0)
    {
        a.valid = false;
        return a;
    }

    int size = lseek(fd, 0, SEEK_END);
    int start;
    int toRead;
    getFileRange(size, mySection - 1, totSections, &start, &toRead);
    lseek(fd, start, 0);
    int i = 0;
    while (i < toRead)
    {
        char cc;
        read(fd, &cc, 1);
        addCharAnalysis(&a, cc);
        i++;
    }
    close(fd);

    return a;
}

void printAnalysis(const int file, Analysis a)
{
    write(file, &a.valid, sizeof(a.valid));
    write(file, a.values, sizeof(a.values));
    write(file, "\n", 1);
}

Analysis readAnalysis(const int file)
{
    Analysis a;
    read(file, &a.valid, sizeof(a.valid));
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

    if (a.valid == false)
        res->valid = false;
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

void printAnalysisReadable(const Analysis a)
{
    fprintf(stderr, "%d - %d - %d - %d - %d - %d - %d - %d - %d - %d\n", (int)a.valid, (int)a.values[0], (int)a.values[1], (int)a.values[2], (int)a.values[3], (int)a.values[4], (int)a.values[5], (int)a.values[6], (int)a.values[7], (int)a.values[8]);
}

void printAnalysisReport(const int fd, Analysis a)
{
    int upperLetter = a.values[AN_UPPLT];
    int lowerLetter = a.values[AN_LOWLT];
    int number = a.values[AN_NUMBR];
    int mathSymbols = a.values[AN_MATHS];
    int punctuation = a.values[AN_PUNCT];
    int brackets = a.values[AN_BRCKT];
    int space = a.values[AN_SPACE];
    int text = a.values[AN_OTEXT];
    int other = a.values[AN_OTHER];

    char line[MAX_CMD_LENGHT];

    if (a.valid)
    {

        sprintf(line, "the total number of character are  %d \n", upperLetter + lowerLetter + number + mathSymbols + punctuation + brackets + space + text + other);
        write(fd, line, strlen(line));

        sprintf(line, "the number of uppercase letters is  %d \n", upperLetter);
        write(fd, line, strlen(line));

        sprintf(line, "the number of lowercase letters is %d \n", lowerLetter);
        write(fd, line, strlen(line));

        sprintf(line, "the number of numbers is %d \n", number);
        write(fd, line, strlen(line));

        sprintf(line, "the number of math character is %d \n", mathSymbols);
        write(fd, line, strlen(line));

        sprintf(line, "the number of punctuation symbols is %d \n", punctuation);
        write(fd, line, strlen(line));

        sprintf(line, "the number of brackets is %d \n", brackets);
        write(fd, line, strlen(line));

        sprintf(line, "the number of spaces is %d \n", space);
        write(fd, line, strlen(line));

        sprintf(line, "other characters( & $ @ _ ^ | #) : %d \n", text);
        write(fd, line, strlen(line));

        sprintf(line, "other like \\n etc %d \n", other);
        write(fd, line, strlen(line));
    }
    else
    {
        strcpy(line, "Errore di lettura del file\n");
        write(fd, line, strlen(line));
    }
}

int isText(char c) { return (c >= ' ' && c <= '~') ? 1 : 0; }
int isUppLetter(char c) { return (c >= 'A' && c <= 'Z') ? 1 : 0; }
int isLowLetter(char c) { return (c >= 'a' && c <= 'z') ? 1 : 0; }
int isNumber(char c) { return (c >= '0' && c <= '9') ? 1 : 0; }
int isMathSymbol(char c) { return (strstr("=<>+-*/", &c) != NULL) ? 1 : 0; }
int isPunctuation(char c) { return (strstr(".,:;'!?`\"", &c) != NULL) ? 1 : 0; }
int isBracket(char c) { return (strstr("()[]{}", &c) != NULL) ? 1 : 0; }
int isSpace(char c) { return (c == ' ') ? 1 : 0; }
