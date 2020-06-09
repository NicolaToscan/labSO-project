#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "lib/commands.h"
#include "lib/common.h"
#include "lib/analisys.h"
#include "lib/communication.h"

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO

#define LEN_FILE MAX_PATH_LENGHT
#define LEN_ANALYSIS ANAL_LENGTH

typedef struct ReportData_s
{
    char *filename;
    Analysis *a;
} ReportData;

ReportData *reportDatas = NULL;
int reportDatasLen = 0;

int tot = 0;
char *fileNames_strings;
int *analysis_elems;
int *deleted;

int READ_A = 0;
bool readingFromA = false;
bool printWhenReady = false;
int fileDone = 0;

void doReadAnalysis();
void deleteFile();
void printReport();
void *readFromA();
void returnBusy();
void addFile(char *file, int fileLen, Analysis an);
void removeFile();
void clean();

int main(int argc, char *argv[])
{
    if (argc >= 2)
        READ_A = atoi(argv[1]);
    else
    {
        char *myfifo = NAMED_PIPE;
        mkfifo(myfifo, 0666);
        READ_A = open(myfifo, O_RDONLY);
    }
    logg("R started");

    pthread_t thredReaderA;
    pthread_create(&thredReaderA, NULL, readFromA, NULL);

    while (true)
    {
        char cmd = readchar(IN);

        switch (cmd)
        {
            // REMOVE FILE
        case CMD_REMOVE_FILE:
            logg("REMOVING");
            removeFile();
            break;

            //REPORT
        case CMD_REQUEST_REPORT:
            printReport();
            clearLine(IN);
            break;

        case CMD_REQUEST_REPORT_WHEN_READDY:
            if (readingFromA)
                printWhenReady = true;
            else
                printReport();
            clearLine(IN);
            break;

            //CLEAN
        case CMD_CLEAN:
            clean();
            clearLine(IN);
            break;

            //KILL
        case CMD_KILL:
            clearLine(IN);
            logg("R KILLED");
            exit(0);
            break;

            //CLEAR LINE
        default:
            clearLine(IN);
            logg("CMD NOT FOUND DA R");
            break;
        }
    }

    return 0;
}

void clean()
{
    if (readingFromA)
    {
        returnBusy();
        return;
    }

    int i;
    for (i = 0; i < reportDatasLen; i++)
    {
        free(reportDatas[i].a);
        free(reportDatas[i].filename);
    }

    free(reportDatas);
    reportDatasLen = 0;
    printSuccess(OUT);
    error("CLEANED");
}

void printReport()
{
    if (readingFromA)
    {
        returnBusy();
        return;
    }

    sendCharCommand(OUT, CMD_REPORT);
    int i;
    loggN(reportDatasLen);
    for (i = 0; i < reportDatasLen; i++)
    {
        write(OUT, "File:\n", strlen("File:\n"));
        write(OUT, reportDatas[i].filename, strlen(reportDatas[i].filename));
        write(OUT, "\n", 1);

        write(OUT, "Analysis:\n", strlen("Analysis:\n"));

        printAnalysisReport(OUT, *(reportDatas[i].a));
    }
    write(OUT, "\n", 1);
}

void returnBusy()
{
    char outStr[12];
    sprintf(outStr, "%c%d\n", CMD_BUSY, fileDone);
    write(OUT, outStr, strlen(outStr));
}

void *readFromA()
{
    char filename[MAX_PATH_LENGHT];
    char cmd;
    int filenameLen;
    Analysis a;

    while (true)
    {
        read(READ_A, &cmd, 1);
        switch (cmd)
        {
        case CMD_START:
            clearLine(READ_A);
            readingFromA = true;
            fileDone = 0;
            break;

        case CMD_FILE:
            filenameLen = readFilename(READ_A, filename);
            a = readAnalysis(READ_A);
            addFile(filename, filenameLen, a);
            fileDone++;
            break;

        case CMD_END:
            readingFromA = false;
            clearLine(READ_A);
            if (printWhenReady)
            {
                printReport();
                printWhenReady = false;
            }
            break;

        default:
            logg("CMD NOT FOUND BY R thread");
            break;
        }
    }
}

void addFile(char *file, int fileLen, Analysis an)
{
    error(file);
    int found = -1;
    int i;
    for (i = 0; i < reportDatasLen; i++)
    {
        if (strcmp(reportDatas[i].filename, file) == 0)
        {
            found = i;
            break;
        }
    }

    ReportData *toUpdate;
    if (found == -1)
    {
        if (reportDatasLen == 0)
            reportDatas = (ReportData *)malloc(sizeof(ReportData));
        else
            reportDatas = (ReportData *)realloc(reportDatas, (reportDatasLen + 1) * (sizeof(ReportData)));
        toUpdate = reportDatas + reportDatasLen;
        reportDatasLen++;
    }
    else
    {
        toUpdate = reportDatas + found;
    }

    toUpdate->a = (Analysis *)malloc(sizeof(Analysis));
    memcpy(toUpdate->a, &an, sizeof(Analysis));

    toUpdate->filename = (char *)malloc((fileLen + 1) * sizeof(char));
    strcpy(toUpdate->filename, file);
    toUpdate->filename[fileLen] = '\0';
}

void removeFile()
{
    char file[MAX_PATH_LENGHT];
    readline(IN, file, MAX_PATH_LENGHT);

    if (readingFromA)
    {
        returnBusy();
        return;
    }

    int found = -1;
    int i;
    for (i = 0; i < reportDatasLen; i++)
    {
        if (strcmp(reportDatas[i].filename, file) == 0)
        {
            found = i;
            break;
        }
    }

    if (found < 0)
    {
        printFail(OUT);
        return;
    }

    free(reportDatas[found].a);
    free(reportDatas[found].filename);

    if (found != 0)
        memcpy(reportDatas, reportDatas, found * sizeof(ReportData));
    if (found != (reportDatasLen - 1))
        memcpy(reportDatas + found, reportDatas + found + 1, (reportDatasLen - found - 1) * sizeof(char *));

    reportDatas = (ReportData *)realloc(reportDatas, (reportDatasLen - 1) * (sizeof(ReportData)));
    reportDatasLen--;
    printSuccess(OUT);
}