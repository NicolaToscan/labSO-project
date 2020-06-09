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
bool updateQnumbers();
void killQ(QData q, int i);
bool forwardFile();

int Q = DEFAULT_Q;
QData *qDatas = NULL;
int qDatasLen = 0;

int main(int argc, char *argv[])
{
    if (argc >= 2)
        Q = atoi(argv[1]);
    if (Q <= 0)
        Q = DEFAULT_Q;

    if (resizeQ(Q))
        printSuccess(OUT);
    else
        printFail(OUT);

    logg("P started");

    while (true)
    {
        char cmd = readchar(IN);
        //fprintf(stderr, " ---- P COMANDO: %c-%d\n", cmd, getpid());

        switch (cmd)
        {
            //UPDATE Qs
        case CMD_P_Qs:
            if (updateQnumbers())
                printSuccess(OUT);
            else
                printFail(OUT);
            clearLine(IN);
            break;

        case CMD_END:
            clearLine(IN);
            sendFine(OUT);
            break;

            //FORWARD FILE
        case CMD_FILE:
            forwardFile();
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
    if (pipe(fdDOWN) == -1)
        return false;
    qData->write = fdDOWN[WRITE];

    int fdUP[2];
    if (pipe(fdUP) == -1)
    {
        close(fdDOWN[WRITE]);
        close(fdDOWN[READ]);
        return false;
    }
    qData->read = fdUP[READ];

    pid_t pid = fork();
    if (pid == 0) //Child
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Istr[9];
        sprintf(Istr, "%d", i + 1);
        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        execlp(FILENAME_Q, FILENAME_Q, Istr, Qstr, (char *)NULL);
        execErrorHandleAndExit(STDOUT_FILENO, fdDOWN[READ], fdUP[WRITE]);
    }
    else if (pid < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        return false;
    }

    qData->pid = pid;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);
    if (readSimpleYNResponce(qData->read))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool resizeQ(int q)
{
    if (qDatasLen == q)
        return true;

    int startError = -1;

    QData *temp = (QData *)malloc(q * sizeof(QData));
    int i;
    if (q > qDatasLen)
    {
        for (i = 0; i < qDatasLen; i++)
            temp[i] = qDatas[i];
        for (i = qDatasLen; i < q; i++)
        {
            if (!startQ(&(temp[i]), i))
            {
                startError = i;
                break;
            }
        }

        // ERROR ON START
        if (startError != -1)
        {
            error("ERROR RESIZING Qs, rolling back");
            for (i = qDatasLen; i <= startError; i++)
                killQ(qDatas[i], i);
            free(temp);
            return false;
        }
    }
    else
    {
        for (i = 0; i < q; i++)
            temp[i] = qDatas[i];
        for (i = q; i < qDatasLen; i++)
            killQ(qDatas[i], i);
    }

    if (qDatas != NULL)
        free(qDatas);
    qDatas = temp;
    qDatasLen = q;

    return true;
}

bool updateQnumbers()
{
    int q = readPQs(IN);
    if (Q == q)
        return true;

    int toUpdate = (q < qDatasLen) ? q : qDatasLen;
    int i = 0;
    for (i = 0; i < toUpdate; i++)
        sendQnumbers(qDatas[i].write, i + 1, q);

    if (resizeQ(q))
    {
        Q = q;
        return true;
    }
    else
    {
        error("P couldn't resize Qs");
        return false;
    }
}

void killQ(QData q, int i)
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

    //SEND
    int i = 0;
    for (i = 0; i < qDatasLen; i++)
        sendFilename(qDatas[i].write, filename, filenameLen);

    //READ
    Analysis tot = initAnalysis();
    for (i = 0; i < qDatasLen; i++)
    {
        Analysis a = readAnalysis(qDatas[i].read);
        sumAnalysis(&tot, a);
    }

    //RESPONSE
    sendFilename(OUT, filename, filenameLen);
    printAnalysis(OUT, tot);
    return true;
}