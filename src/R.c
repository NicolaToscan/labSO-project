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

int tot = 0;
char *fileNames_strings;
int *analysis_elems;
int *deleted;

int READ_A = 0;
bool readingFromA = false;

void doReadAnalysis();
void deleteFile();
void printReport();
void *readFromA();

int main(int argc, char *argv[])
{
    if (argc >= 22)
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

            //ANALYSIS
        case CMD_ANALYSIS:
            doReadAnalysis();
            break;

            // REMOVE FILE
        case CMD_REMOVE_FILE:
            deleteFile();
            break;

            //REPORT
        case CMD_REQUEST_REPORT:
            printReport();
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

void doReadAnalysis()
{
    char fileName[MAX_PATH_LENGHT];
    readFilename(IN, fileName);
    Analysis a = readAnalysis(IN);

    tot++;

    // Store Nome file
    fileNames_strings = realloc(fileNames_strings, tot * LEN_FILE * sizeof(char));
    strcpy((LEN_FILE * (tot - 1)) + fileNames_strings, fileName);

    // Store Analisi
    analysis_elems = realloc(analysis_elems, tot * LEN_ANALYSIS * sizeof(uint32));
    int temp = LEN_ANALYSIS * (tot - 1);
    int i;
    for (i = 0; i < LEN_ANALYSIS; i++)
    {
        analysis_elems[i + temp] = a.values[i];
    }

    // Store Deleted
    deleted = realloc(deleted, tot * sizeof(int));
    deleted[tot - 1] = 0;

    clearLine(IN);
}

void deleteFile()
{
    char fileName[MAX_PATH_LENGHT];
    readFilename(IN, fileName);

    char *find = strstr(fileNames_strings, fileName);
    int pos = (int)(find - fileNames_strings);

    if (find == NULL)
    {
        error("Impossibile eliminare file non presente");

        // Va gestito???
    }
    else
    {
        int index = pos / LEN_FILE;
        deleted[index] = 1;
    }

    clearLine(IN);
}

void printReport()
{
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
            readingFromA = true;
            clearLine(READ_A);
            logg("READING STARTED FROM R");
            break;

        case CMD_FILE:
            filenameLen = readFilename(READ_A, filename);
            a = readAnalysis(READ_A);
            logg("GOTTAFILE");
            logg(filename);
            break;

        case CMD_END:
            readingFromA = false;
            clearLine(READ_A);
            logg("READING ENDED FROM R");
            break;

        default:
            logg("CMD NOT FOUND BY R thread");
            break;
        }
    }
}
