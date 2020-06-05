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

typedef struct FileDataList_s
{
    char *filename;
    Analysis *analysis;
    struct FileDataList_s *next;
} FileDataList;

int N = 1;
int M = 1;

FileDataList *files = NULL;

void readCommand();
void addFile(char *f);
void removeFile(char *f);
void printFiles();

int main(int argc, char *argv[])
{
    logg("A started");
    while (true)
    {
        printFiles();
        readCommand();
    }
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
    else if (strcmp(cmds[0], "F") == 0) //ADD FILE
    {
        if (num == 2)
        {
            addFile(cmds[1]);
            printSuccess(OUT);
        }
        else
            printFail(OUT);
    }
    else if (strcmp(cmds[0], "R") == 0) //REMOVE FILE
    {
        if (num == 2)
        {
            removeFile(cmds[1]);
            printSuccess(OUT);
        }
        else
            printFail(OUT);
    }
    else
    {
        printFail(OUT);
    }

    free(inLine);
    free(cmds);
}

void addFile(char *f)
{
    if (files == NULL)
    {
        FileDataList *fd = (FileDataList *)malloc(sizeof(FileDataList));
        fd->analysis = NULL;
        fd->filename = (char *)malloc(MAX_PATH_LENGHT * sizeof(char));
        strcpy(fd->filename, f);
        files = fd;
        return;
    }

    FileDataList *curr = files;

    if (curr != NULL)
        do
        {
            if (strcmp(curr->filename, f) == 0)
                return;
            if (curr->next == NULL)
                break;
            curr = curr->next;
        } while (true);

    FileDataList *fd = (FileDataList *)malloc(sizeof(FileDataList));
    fd->analysis = NULL;
    fd->filename = (char *)malloc(MAX_PATH_LENGHT * sizeof(char));
    strcpy(fd->filename, f);
    curr->next = fd;
}

void removeFile(char *f)
{
    FileDataList *prev = NULL;
    FileDataList *curr = files;

    while (curr != NULL)
    {
        if (strcmp(curr->filename, f) == 0)
        {
            if (prev == NULL)
                files = curr->next;
            else
                prev->next = curr->next;

            free(curr->analysis);
            free(curr->filename);
            free(curr);
            break;
        }

        prev = curr;
        curr = curr->next;
    }
}

void printFiles()
{
    logg("FILES");
    FileDataList *curr = files;
    while (curr != NULL)
    {
        write(OUT, curr->filename, strlen(curr->filename));
        write(OUT, "\n", 1);
        curr = curr->next;
    }
}