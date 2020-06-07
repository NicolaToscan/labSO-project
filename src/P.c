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

bool startQ(QData *qData, int i);
bool resizeQ(int q);
void updateQnumbers();
void killQ(QData q);
bool forwardFile();

int Q = 4;
QData *qDatas = NULL;
int qDatasLen = 0;

int main(int argc, char *argv[])
{
    if (argc >= 2)
        Q = atoi(argv[1]);
    if (Q <= 0)
        Q = 4;

    resizeQ(Q);
    logg("P started");

    while (true)
    {
        char cmd = readchar(IN);

        switch (cmd)
        {
            //UPDATE Qs
        case CMD_P_Qs:
            updateQnumbers();
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
            resizeQ(0);
            logg("P KILLED");
            exit(0);
            break;

            //CLEAR LINE
        default:
            clearLine(IN);
            logg("CMD NOT FOUND DA P");
            exit(0);
            break;
        }
    }

    return 0;
}

bool startQ(QData *qData, int i)
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
    }
    else if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Istr[9];
        sprintf(Istr, "%d", i);
        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        //close(fdDOWN[READ]);
        //close(fdUP[WRITE]);
        execlp(FILENAME_Q, FILENAME_Q, Istr, Qstr, (char *)NULL);
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

bool resizeQ(int q)
{
    if (qDatasLen == q)
        return true;

    int i;
    if (q > qDatasLen)
    {
        qDatas = (QData *)realloc(qDatas, q * sizeof(QData *));
        for (i = qDatasLen; i < q; i++)
            startQ(&(qDatas[i]), i);
    }
    else
    {
        for (i = q; i < qDatasLen; i++)
            killQ(qDatas[i]);
        QData *temp = (QData *)malloc(q * sizeof(QData *));
        memcpy(temp, qDatas, q * sizeof(QData *));
        free(qDatas);
        qDatas = temp;
    }
    qDatasLen = q;
    return true;
}

void updateQnumbers()
{
    int q = readPQs(IN);
    Q = q;

    int toUpdate = (q < qDatasLen) ? q : qDatasLen;
    int i = 0;
    for (i = 0; i < toUpdate; i++)
        sendQnumbers(qDatas[i].write, i, q);

    resizeQ(Q);
}

void killQ(QData q)
{
    sendKill(q.write);
    close(q.write);
    close(q.read);
}

bool forwardFile()
{
    if (qDatasLen == 0)
        return false;

    char filename[MAX_PATH_LENGHT];
    int filenameLen = readFilename(IN, filename);

    int i = 0;
    for (i = 0; i < qDatasLen; i++)
        sendFilename(qDatas[i].write, filename, filenameLen);
}
