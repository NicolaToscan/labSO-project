#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define in stdin
#define OUT STDOUT_FILENO

typedef struct FileData_s
{
    char *filename;
    Analysis *Analysis;
} FileData;

int N = 1;
int M = 1;

FileData *files;

void readCommand();

int main(int argc, char *argv[])
{
    logg("A started");
}

void readCommand()
{
    size_t buffSize = MAX_CMD_LENGHT;
    char *inLine = NULL;
    int len = getline(&inLine, &buffSize, in);
    inLine[--len] = '\0';
    char **cmds = NULL;
    char *p = strtok(inLine, " ");
    int num = 0;

    while (p)
    {
        cmds = realloc(cmds, sizeof(char *) * ++num);
        if (cmds == NULL)
        {
            //TODO: MERDA
            exit(0);
        }
        cmds[num - 1] = p;
        p = strtok(NULL, " ");
    }
    optind = 1; //RESET COSO CHE LETTE OPTs

    if (strcmp(cmds[0], "P") == 0)
    {
        if (num == 3)
        {
            int m = atoi(cmds[1]);
            int n = atoi(cmds[2]);
            if (m > 0 && n > 0)
            {
                N = n;
                M = m;
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else
            printFail(OUT);
    }
    else if (strcmp(cmds[0], "F") == 0)
    {
        addFile(num, cmds);
    }
    else
    {
        printFail(OUT);
    }

    free(inLine);
    free(cmds);
}

void addFile()
{
}