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

bool startP(PData *pData);
bool resizeP(int toAdd);
void killP(PData *p);

int P = 3;
int Q = 4;
PData *pDatas = NULL;

int main(int argc, char *argv[])
{
    if (argc >= 2)
        P = atoi(argv[1]);
    if (argc >= 3)
        Q = atoi(argv[2]);

    if (P <= 0)
        P = 3;
    if (Q <= 0)
        Q = 4;

    resizeP(P);

    logg("C started");


    while (true)
    {
        char cmd = readchar(IN);
        switch (cmd)
        {
            //UPDATE Q and P
        case CMD_C_PandQ:
            updatePandQ();
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

bool startP(PData *pData)
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
    }
    else if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        //close(fdDOWN[READ]);
        //close(fdUP[WRITE]);
        execlp(FILENAME_P, FILENAME_P, Qstr, (char *)NULL);
        //TODO: handle exec error
        error("EXEC error");
    }
    else
    {
        //TODO: handle fork error
        error("FORK error");
        return false;
    }
    return true;
}

bool resizeP(int toAdd)
{
    if (toAdd == 0)
        return true;

    if (currentPData == NULL)
    {

        currentPData = (PData *)malloc(sizeof(PData));
        if (!startP(currentPData))
        {
            free(currentPData);
            currentPData = NULL;
            return false;
        }
        currentPData->next = currentPData;
        currentPData->prev = currentPData;
    }

    int i = 0;
    if (toAdd > 0)
    {
        for (i = 0; i < toAdd; i++)
        {
            PData *nuovo = (PData *)malloc(sizeof(PData));
            if (startP(currentPData))
            {
                nuovo->next = currentPData->next;
                currentPData->next->prev = nuovo;
                nuovo->prev = currentPData;
                currentPData->next = nuovo;
            }
            {
                free(currentPData);
                currentPData = NULL;
                return false;
            }
        }
    }
    else
    {
        toAdd *= -1;
        for (i = 0; i < toAdd; i++)
        {
            killP(currentPData->next);
            currentPData->next->next->prev = currentPData;
            free(currentPData->next);
            currentPData->next = currentPData->next->next;
        }
    }
    return true;
}

void updatePandQ()
{
    int p, q;
    readPandQ(IN, &p, &q);
    Q = q;

     



    resizePs(p - P);
    P = p;
}

void killP(PData *p)
{
    sendKill(p->write);
    close(p->write);
    close(p->read);
}


void forwardFile(PData *P, int pLen, int *toSendFile)
{
    char filename[MAX_PATH_LENGHT];
    int filenameLen = readFilename(IN, filename);

    *toSendFile = (*toSendFile + 1) % pLen;
    sendFilename(P[*toSendFile].write, filename, filenameLen);
}

