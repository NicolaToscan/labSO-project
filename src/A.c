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

int WRITE_C = 0;
int READ_C = 0;
int PID_C = 0;

int READ_REPORTER = -1;
bool isReporting = false;

FileDataList *files = NULL;

void startC();
void readCommand();
void addFile(char *f);
void removeFile(char *f);
void toUpdateFile(char *f);
void printFiles();
void startAReport();
bool checkFileExist(char *f);

int main(int argc, char *argv[])
{
    startC();
    logg("A started");
    while (true)
    {
        //printFiles();
        readCommand();
    }
    return 0;
}

void startC()
{
    int fdDOWN[2];
    pipe(fdDOWN);
    WRITE_C = fdDOWN[WRITE];

    int fdUP[2];
    pipe(fdUP);
    READ_C = fdUP[READ];

    pid_t pid = fork();
    if (pid > 0) // Parent
    {
        PID_C = pid;
        close(fdDOWN[READ]);
        close(fdUP[WRITE]);
    }
    else if (pid == 0)
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        if (execlp(FILENAME_C, FILENAME_C, (char *)NULL) < 0)
        {
            //TODO: handle exec error
            error("EXEC error");
        }
    }
    else
    {
        //TODO: handle fork error
        error("FORK error");
    }
}

void readCommand()
{
    size_t buffSize = MAX_CMD_LENGHT;
    char *inLine = NULL;
    int len = getline(&inLine, &buffSize, in);
    if (len <= 1)
        return;
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

    if (num > 0)
    {
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
        else if (strcmp(cmds[0], "A") == 0) //ADD FILE
        {
            if (num == 2 && checkFileExist(cmds[1]))
            {
                addFile(cmds[1]);
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "R") == 0) //REMOVE FILE
        {
            if (num == 2 && checkFileExist(cmds[1]))
            {
                removeFile(cmds[1]);
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "U") == 0) //TO UPDATE FILE
        {
            if (num == 2 && checkFileExist(cmds[1]))
            {
                toUpdateFile(cmds[1]);
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "L") == 0) //LIST FILE
        {
            printFiles();
        }
        else if (strcmp(cmds[0], "S") == 0) //START REPORT
        {
            logg("REPORT STARTED");
            startAReport();
            printSuccess(OUT);
        }
        else if (strcmp(cmds[0], "K") == 0) //KILL
        {
            loggN(WRITE_C);
            sendKill(WRITE_C);
            logg("A killed");
            exit(0);
        }
        else
        {
            printFail(OUT);
        }
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

void toUpdateFile(char *f)
{
    FileDataList *curr = files;

    while (curr != NULL)
    {
        if (strcmp(curr->filename, f) == 0)
        {
            free(curr->analysis);
            curr->analysis = NULL;
            break;
        }
        curr = curr->next;
    }
}

void printFiles()
{
    FileDataList *curr = files;
    while (curr != NULL)
    {
        //logg(curr->filename);
        write(OUT, curr->filename, strlen(curr->filename));
        write(OUT, "\n", 1);
        curr = curr->next;
    }
    write(OUT, "\n", 1);
}

bool checkFileExist(char *f)
{
    bool trovato = false;
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if (pid > 0) //PARENT
    {
        close(fd[WRITE]);
        char buff[MAX_PATH_LENGHT];
        int letti = read(fd[READ], buff, MAX_PATH_LENGHT);
        if (letti == 0)
            trovato = true;
        close(fd[READ]);
    }
    else if (pid == 0) //CHILD
    {
        close(fd[READ]);
        dup2(fd[WRITE], STDERR_FILENO);
        dup2(open("/dev/null", O_WRONLY), STDOUT_FILENO);

        execl("/usr/bin/find", "/usr/bin/find", f, NULL);
        error("EXEC ERROR");
    }
    else
    {
        error("FORK ERROR");
    }
    return trovato;
}


// ----- REPORT STUFF -----
void doReport();

void startAReport()
{
    if (READ_REPORTER >= 0)
    {
        close(READ_REPORTER);
    }

    int pipeToReport[2];
    pipe(pipeToReport);

    READ_REPORTER = pipeToReport[READ];
    int WRITE_TO_PARENT = pipeToReport[WRITE];

    isReporting = true;
    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipeToReport[READ]);

        doReport();

        char resOK[2] = {RESPONSE_OK, '\n'};
        write(WRITE_TO_PARENT, resOK, 2);
        close(pipeToReport[WRITE]);
        exit(0);
    }
    close(pipeToReport[WRITE]);
    char res[2];
    read(READ_REPORTER, res, 2);
    isReporting = false;
    logg("REPORT FINITO");
}

void doReport()
{
    //SET P & Q
    sendPandQ(WRITE_C, M, N);



}