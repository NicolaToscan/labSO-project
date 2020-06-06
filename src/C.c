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

typedef struct PData_s
{
    int write;
    int read;
    int pid;

} PData;

void startP(PData *pData, int qs);
PData *resizePs(const int newN, int *nCurr, PData *P, int qs);
void resetP(const int write, const int read, const int qs);
void killP(const int write, const int read);
void forwardFile(PData *P, int pLen, int *toSendFile);
void updatePandQ(PData *P, int *nCurr, int *qs);

int main(int argc, char *argv[])
{
    logg("C started");

    int qs = 1;
    int currentPs = 1;
    int toSendFile = 0;
    PData *P = calloc(currentPs, sizeof(PData));
    startP(P, qs);

    while (true)
    {
        char cmd = readchar(IN);
        switch (cmd)
        {
            //UPDATE Q and P
        case CMD_C_PandQ:
            updatePandQ(P, &currentPs, &qs);
            clearLine(IN);
            break;

            //FORWARD FILE
        case CMD_FILE:
            forwardFile(P, currentPs, &toSendFile);
            clearLine(IN);
            break;

            //KILL
        case CMD_KILL:
            clearLine(IN);
            resizePs(0, &currentPs, P, qs);
            logg("C KILLED");
            exit(0);
            break;

            //CLEAR LINE
        default:
            clearLine(IN);
            logg("CMD NOT FOUND DA C");
            break;
        }
    }

    return 0;
}

void updatePandQ(PData *P, int *nCurr, int *qs)
{
    int p, q;
    readPandQ(IN, &p, &q);
    resizePs(p, nCurr, P, q);
    *qs = q;
}

void forwardFile(PData *P, int pLen, int *toSendFile)
{
    char filename[MAX_PATH_LENGHT];
    int filenameLen = readFilename(IN, filename);

    *toSendFile = (*toSendFile + 1) % pLen;
    sendFilename(P[*toSendFile].write, filename, filenameLen);
}

void startP(PData *pData, int qs)
{
    int fdDOWN[2];
    pipe(fdDOWN);
    pData->write = fdDOWN[WRITE];

    int fdUP[2];
    pipe(fdUP);
    pData->read = fdUP[READ];

    pid_t pid = fork();
    if (pid > 0) //Parent
    {
        pData->pid = pid;
        close(fdDOWN[READ]);
        close(fdUP[WRITE]);
        resetP(pData->write, pData->read, qs);
    }
    else if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        //close(fdDOWN[READ]);
        //close(fdUP[WRITE]);

        if (execlp(FILENAME_P, FILENAME_P, (char *)NULL) < 0)
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

PData *resizePs(const int newN, int *nCurr, PData *P, int qs)
{
    //TODO: forse realloc
    if (newN == *nCurr)
        return P;

    PData *newP = (PData *)calloc(newN, sizeof(PData));

    int toCopy = newN < *nCurr ? newN : *nCurr;
    int i = 0;
    for (i = 0; i < toCopy; i++)
    {
        newP[i].read = P[i].read;
        newP[i].write = P[i].write;
        newP[i].pid = P[i].pid;
        resetP(newP[i].write, newP[i].read, qs);
    }

    if (newN > *nCurr)
    {
        for (; i < newN; i++)
            startP(newP + i, qs);
    }
    else //(n < nCurr)
    {
        for (; i < *nCurr; i++)
            killP(P[i].write, P[i].read);
    }
    free(P);
    *nCurr = newN;
    return newP;
}

void resetP(const int write, const int read, const int qs)
{
    sendPQs(write, qs);
}

void killP(const int write, const int read)
{
    sendKill(write);
    //TODO close pipe
}
