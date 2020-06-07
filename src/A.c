#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/analisys.h"
#include <pthread.h>
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define in stdin
#define OUT STDOUT_FILENO

char **filenames;
int filenamesLen = 0;

int P = 2;
int Q = 4;

int WRITE_C = 0;
int READ_C = 0;
int PID_C = 0;

int READ_REPORTER = -1;
bool isReporting = false;

void startC();
void readCommand();
void addFile(char *f);
void removeFile(char *f);
void printFiles();
bool startAReport();
bool checkFileExist(char *f);

int main(int argc, char *argv[])
{
    startC();
    logg("A started");
    while (true)
        readCommand();
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

        char Pstr[9];
        sprintf(Pstr, "%d", P);
        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        if (execlp(FILENAME_C, FILENAME_C, Pstr, Qstr, (char *)NULL) < 0)
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
                int p = atoi(cmds[1]);
                int q = atoi(cmds[2]);
                if (p > 0 && q > 0)
                {
                    P = p;
                    Q = q;
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
        else if (strcmp(cmds[0], "L") == 0) //LIST FILE
        {
            printFiles();
        }
        else if (strcmp(cmds[0], "S") == 0) //START REPORT
        {
            startAReport();
            printSuccess(OUT);
        }
        else if (strcmp(cmds[0], "K") == 0) //KILL
        {
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
    int i;
    for (i = 0; i < filenamesLen; i++)
        if (strcmp(filenames[i], f) == 0)
            return;

    filenames = (char **)realloc(filenames, ++filenamesLen * sizeof(char *));
    filenames[filenamesLen - 1] = malloc(MAX_PATH_LENGHT * sizeof(char));
    strcpy(filenames[filenamesLen - 1], f);
}

void removeFile(char *f)
{
    int found;
    for (found = 0; found < filenamesLen; found++)
        if (strcmp(filenames[found], f) == 0)
            break;

    logg("FOUND");
    loggN(found);
    if (found == filenamesLen)
        return;

    char **temp = (char **)malloc((filenamesLen - 1) * sizeof(char *));

    if (found != 0)
        memcpy(temp, filenames, found * sizeof(char *));
    if (found != (filenamesLen - 1))
        memcpy(temp + found, filenames + found + 1, (filenamesLen - found - 1) * sizeof(char *));

    filenamesLen--;
    free(filenames[found]);
    free(filenames);
    filenames = temp;
}

void printFiles()
{
    int i;
    for (i = 0; i < filenamesLen; i++)
    {
        //logg(curr->filename);
        write(OUT, filenames[i], strlen(filenames[i]));
        write(OUT, "\n", 1);
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
void *sendStuff();
void *readStuff();

char **toFindArg;
int toFindArgLen = 0;

bool startAReport()
{
    if (isReporting)
        return false;
    if (filenamesLen == 0)
        return true;

    toFindArgLen = filenamesLen + 2;
    toFindArg = (char **)malloc(toFindArgLen * sizeof(char *));
    int i;
    for (i = 0; i < filenamesLen; i++)
        toFindArg[i + 1] = filenames[i];
    toFindArg[filenamesLen + 1] = NULL;

    free(filenames);
    filenamesLen = 0;
    filenames = (char **)malloc(0);

    isReporting = true;
    pthread_t thredSender, thredReciver;
    pthread_create(&thredSender, NULL, sendStuff, NULL);
    // pthread_create(&thredSender, NULL, readStuff, NULL);

    return true;
}

void *sendStuff()
{
    // SET P & Q
    sendPandQ(WRITE_C, P, Q);

    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0) //CHILD
    {
        close(fd[READ]);
        dup2(fd[WRITE], STDOUT_FILENO);
        dup2(open("/dev/null", O_WRONLY), STDERR_FILENO);
        toFindArg[0] = "/usr/bin/find";
        execv("/usr/bin/find", toFindArg);
        error("EXEC ERROR");
    }
    else if (pid < 0)
    {
        error("FORK ERROR");
    }

    //PARENT
    close(fd[WRITE]);
    char line[MAX_PATH_LENGHT];
    int letti;
    char c;
    int i = 0;
    while ((letti = read(fd[READ], &c, 1)) > 0)
    {
        line[i] = c;
        if (c == '\n')
        {
            line[i] = '\0';
            if (i > 0)
                sendFilename(WRITE_C, line, strlen(line));
            i = 0;
        }
        else
            i++;
    }
    logg("INVIATO REPORT");

    close(fd[READ]);

    //CLEAR
    for (i = 0; i < toFindArgLen; i++)
        free(toFindArg[i]);
    free(toFindArg);
    toFindArgLen = 0;

    isReporting = false;
}

void *readStuff()
{
}