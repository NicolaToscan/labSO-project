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
void killP(PData p);
bool forwardFile();
void updatePandQ();
void *forwardUpReports();

int P = 3;
int Q = 4;
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
        P = 3;
    if (Q <= 0)
        Q = 4;

    resizeP(P);

    logg("C started");

    while (true)
    {
        char cmd = readchar(IN);
        //fprintf(stderr, " -- C COMANDO: %c-%d\n", cmd, getpid());

        switch (cmd)
        {
            //UPDATE Q and P
        case CMD_C_PandQ:
            updatePandQ();
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

bool resizeP(int p)
{
    if (pDatasLen == p)
        return true;

    PData *temp = (PData *)malloc(p * sizeof(PData));
    int i;
    if (p > pDatasLen)
    {
        for (i = 0; i < pDatasLen; i++)
            temp[i] = pDatas[i];
        for (i = pDatasLen; i < p; i++)
            startP(&(temp[i]));
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

void updatePandQ()
{
    int p, q;
    readPandQ(IN, &p, &q);
    Q = q;
    P = p;

    int toUpdate = (p < pDatasLen) ? p : pDatasLen;
    int i = 0;
    for (i = 0; i < toUpdate; i++)
        sendPQs(pDatas[i].write, Q);

    resizeP(P);
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