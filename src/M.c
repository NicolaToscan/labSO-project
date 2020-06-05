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
#define out stdout

#define MAX_CMD_LENGHT 8192

void readCommand();
void setCmd(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    while (true)
    {
        printf("# ");
        readCommand();
    }

    return 0;
}

void readCommand()
{
    size_t buffSize = MAX_CMD_LENGHT;
    char *inLine;
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

    if (strcmp(cmds[0], "set") == 0)
    {
        setCmd(num, cmds);
    }
    else if (strcmp(cmds[0], "file") == 0)
    {
    }
    else if (strcmp(cmds[0], "report") == 0)
    {
    }
    else if (strcmp(cmds[0], "help") == 0)
    {
    }
    else if (strcmp(cmds[0], "quit") == 0)
    {
    }
    else
    {
        printf("Command '%s' not found, type help\n", cmds[0]);
    }
}

void setCmd(int argc, char *argv[])
{
    char c;
    while ((c = getopt(argc, argv, "n:m:")) != -1)
    {
        switch (c)
        {
        case 'n':
            printf("N: %s\n", optarg);
            break;
        case 'm':
            printf("N: %s\n", optarg);
            break;

        default:
            break;
        }
    }
}
