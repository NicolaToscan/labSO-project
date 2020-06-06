#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO

typedef struct QData_s
{
    int write;
    int read;
    int pid;

} QData;

void startQ(QData *qData, const int section, const int sections);
QData *resizeQs(const int newN, int *nCurr, QData *Q);
void resetQ(const int write, const int read, const int section, const int sections);
void killQ(const int write, const int read);
void forwardFile(QData *Q, int qLen);

int main(int argc, char *argv[])
{
	logg("P started");

    int currentQs = 1;
    QData *Q = calloc(currentQs, sizeof(QData));
    startQ(Q, 1, 1);

    while (true)
    {
        char cmd = readchar(IN);

        switch (cmd)
        {
            //UPDATE Qs
        case CMD_P_Qs:
            resizeQs(readPQs(IN), &currentQs, Q);
            clearLine(IN);
            break;

            //FORWARD FILE
        case CMD_FILE:
            forwardFile(Q, currentQs);
            clearLine(IN);
            break;

            //KILL
        case CMD_KILL:
            clearLine(IN);
            resizeQs(0, &currentQs, Q);
            logg("P KILLED");
            exit(0);
            break;

            //CLEAR LINE
        default:
            clearLine(IN);
            logg("CMD NOT FOUND");
            break;
        }
    }

    return 0;
}

void forwardFile(QData *Q, int qLen)
{
    char filename[MAX_PATH_LENGHT];
    int filenameLen = readFilename(IN, filename);

    int i = 0;
    for (i = 0; i < qLen; i++)
        sendFilename(Q[i].write, filename, filenameLen);
}

void startQ(QData *qData, const int section, const int sections)
{
    int fdDOWN[2];
    pipe(fdDOWN);
    qData->write = fdDOWN[WRITE];

    int fdUP[2];
    pipe(fdUP);
    qData->read = fdUP[READ];

    pid_t pid = fork();
    if (pid > 0) //Parent
    {
        qData->pid = pid;
        close(fdDOWN[READ]);
        close(fdUP[WRITE]);
        resetQ(qData->write, qData->read, section, sections);
    }
    else if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        //close(fdDOWN[READ]);
        //close(fdUP[WRITE]);

        if (execlp(FILENAME_Q, FILENAME_Q, (char *)NULL) < 0)
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

QData *resizeQs(const int newN, int *nCurr, QData *Q)
{
    //TODO: forse realloc
    if (newN == *nCurr)
        return Q;

    QData *newQ = (QData *)calloc(newN, sizeof(QData));

    int toCopy = newN < *nCurr ? newN : *nCurr;
    int i = 0;
    for (i = 0; i < toCopy; i++)
    {
        newQ[i].read = Q[i].read;
        newQ[i].write = Q[i].write;
        newQ[i].pid = Q[i].pid;
        resetQ(newQ[i].write, newQ[i].read, i + 1, newN);
    }

    if (newN > *nCurr)
    {
        for (; i < newN; i++)
            startQ(newQ + i, i + 1, newN);
    }
    else //(n < nCurr)
    {
        for (; i < *nCurr; i++)
            killQ(Q[i].write, Q[i].read);
    }
    free(Q);
    *nCurr = newN;
    return newQ;
}

void resetQ(const int write, const int read, const int section, const int sections)
{
    if (section > sections)
    {
        error("Index bigger that range");
        exit(12345); //TODO: fix
    }

    sendQnumbers(write, section, sections);
}

void killQ(const int write, const int read)
{
    sendKill(write);
    //TODO close pipe
}
