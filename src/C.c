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

bool startP(PData *pData, int i);
bool resizeP(int toAdd);
void killP(PData p);
bool forwardFile();
bool updatePandQ();
void *forwardUpReports();

int P = DEFAULT_P;
int Q = DEFAULT_Q;
PData *pDatas = NULL;
int pDatasLen = 0;
int pRotation = 0;
int pReadRotation = 0;

int main(int argc, char *argv[])
{
    if (argc >= 2)
        P = atoi(argv[1]);
    if (argc >= 3)
        Q = atoi(argv[2]);

    if (P <= 0)
        P = DEFAULT_P;
    if (Q <= 0)
        Q = DEFAULT_Q;

    if (resizeP(P))
        printSuccess(OUT);
    else
        printFail(OUT);

    logg("C started");

    while (true)
    {
        char cmd = readchar(IN);
        //fprintf(stderr, " -- C COMANDO: %c-%d\n", cmd, getpid());

        switch (cmd)
        {
            //UPDATE Q and P
        case CMD_C_PandQ:
            if (updatePandQ())
                printSuccess(OUT);
            else
                printFail(OUT);
            clearLine(IN);
            break;

            //START SENDING BACK RESULTS
        case CMD_START:
            clearLine(IN);
            pthread_t th;
            pRotation = 0;
            pReadRotation = 0;
            pthread_create(&th, NULL, forwardUpReports, NULL);
            break;

            //FORWARD FILE
        case CMD_FILE:
            forwardFile();
            break;

        case CMD_END:
            clearLine(IN);
            sendFine(pDatas[pRotation].write);
            pRotation = 0;
            break;

            //KILL
        case CMD_KILL:
            clearLine(IN);
            resizeP(0);
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

bool startP(PData *pData, int i)
{
    int fdDOWN[2];
    if (pipe(fdDOWN) == -1)
        return false;
    pData->write = fdDOWN[WRITE];

    int fdUP[2];
    if (pipe(fdUP) == -1)
    {
        close(fdDOWN[WRITE]);
        close(fdDOWN[READ]);
        return false;
    }
    pData->read = fdUP[READ];

    pid_t pid = fork();
    if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        //close(fdDOWN[READ]);
        //close(fdUP[WRITE]);
        char execFilenam[MAX_PATH_LENGHT];
        getExecFilename(FILENAME_P, execFilenam);
        execlp(execFilenam, execFilenam, Qstr, (char *)NULL);
        execErrorHandleAndExit(STDOUT_FILENO, fdDOWN[READ], fdUP[WRITE]);
    }
    else if (pid < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        return false;
    }

    pData->pid = pid;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);
    if (readSimpleYNResponce(pData->read))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool resizeP(int p)
{

    if (pDatasLen == p)
        return true;

    int startError = -1;

    PData *temp = (PData *)malloc(p * sizeof(PData));
    int i;
    if (p > pDatasLen)
    {
        for (i = 0; i < pDatasLen; i++)
            temp[i] = pDatas[i];
        for (i = pDatasLen; i < p; i++)
        {
            if (!startP(&(temp[i]), i))
            {
                startError = i;
                break;
            }
        }

        // ERROR ON START
        if (startError != -1)
        {
            error("ERROR RESIZING Ps, rolling back");
            for (i = pDatasLen; i <= startError; i++)
                killP(pDatas[i]);
            free(temp);
            return false;
        }
    }
    else
    {
        for (i = 0; i < p; i++)
            temp[i] = pDatas[i];
        for (i = p; i < pDatasLen; i++)
            killP(pDatas[i]);
    }

    if (pDatas != NULL)
        free(pDatas);
    pDatas = temp;
    pDatasLen = p;

    return true;
}

bool updatePandQ()
{
    int p, q;
    readPandQ(IN, &p, &q);
    if (p == P && q == Q)
        return true;

    int toUpdate = (p < pDatasLen) ? p : pDatasLen;
    int i = 0;
    int errorI = -1;
    for (i = 0; i < toUpdate; i++)
    {
        sendPQs(pDatas[i].write, q);
        if (!readSimpleYNResponce(pDatas[i].read))
        {
            errorI = i;
            break;
        }
    }

    //ERROR, roll back
    if (errorI != -1)
    {
        error("ROLLING BACK");
        for (i = 0; i <= errorI; i++)
            sendPQs(pDatas[i].write, Q);
    }

    if (resizeP(p))
    {
        Q = q;
        P = p;
        return true;
    }
    else
    {
        error("C couldn't resize Ps");
        return false;
    }
}

void killP(PData p)
{
    sendKill(p.write);
    close(p.write);
    close(p.read);
}

bool forwardFile()
{
    if (pDatasLen == 0)
        return false;

    char filename[MAX_PATH_LENGHT];
    int filenameLen = readFilename(IN, filename);
    sendFilename(pDatas[pRotation].write, filename, filenameLen);

    pRotation = (pRotation + 1) % pDatasLen;
    return true;
}

void *forwardUpReports()
{
    char filename[MAX_PATH_LENGHT];

    char cmd = 'P';
    while (true)
    {
        read(pDatas[pReadRotation].read, &cmd, 1);
        if (cmd == CMD_END)
        {
            read(pDatas[pReadRotation].read, &cmd, 1); // READ \n
            sendFine(OUT);
            return;
        }
        else if (cmd == CMD_FILE)
        {
            int filenameLen = readFilename(pDatas[pReadRotation].read, filename);
            Analysis a = readAnalysis(pDatas[pReadRotation].read);
            sendFilename(OUT, filename, filenameLen);
            printAnalysis(OUT, a);
        }
        pReadRotation = (pReadRotation + 1) % pDatasLen;
    }
}