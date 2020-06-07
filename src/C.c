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

int P = 3;
int Q = 4;
PData *pDatas = NULL;
int pDatasLen = 0;
int pRotation = 0;

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
            forwardFile();
            clearLine(IN);
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

    int i;
    if (p > pDatasLen)
    {
        pDatas = (PData *)realloc(pDatas, p * sizeof(PData *));
        for (i = pDatasLen; i < p; i++)
            startP(&(pDatas[i]));
    }
    else
    {
        for (i = p; i < pDatasLen; i++)
            killP(pDatas[i]);
        PData *temp = (PData *)malloc(p * sizeof(PData *));
        memcpy(temp, pDatas, p * sizeof(PData *));
        free(pDatas);
        pDatas = temp;
    }
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

    pRotation = (pRotation + 1) % pDatasLen;
    sendFilename(pDatas[pDatasLen].write, filename, filenameLen);
}
