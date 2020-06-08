#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/commands.h"
#include "lib/common.h"
#include "lib/analisys.h"
#include "lib/communication.h"

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO

#define LEN_FILE MAX_PATH_LENGTH
#define LEN_ANALYSIS ANAL_LENGTH

int tot = 0;
char *fileNames;
int *analysis;
int *deleted;


void readAnalysis();
void deleteFile();
void printReport();

int main(int argc, char *argv[])
{

    logg("R started");

	while (true)
	{
		char cmd = readchar(IN);

		switch (cmd)
		{

			//ANALYSIS
		case CMD_ANALYSIS:
			readAnalysis();
			break;
        
            // REMOVE FILE
        case CMD_REMOVE_FILE:
            deleteFile();
            break;

			//REPORT
		case CMD_REQUEST_REPORT:
            void printReport();
            clearLine(IN);
			break;

			//KILL
		case CMD_KILL:
			clearLine(IN);
			logg("Q KILLED");
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

void readAnalysis()
{
    char fileName[MAX_PATH_LENGHT];
    readFileName(IN, fileName);
    Analysis a = readAnalysis(fileName);

    tot++;

    // Store Nome file
    fileNames = realloc(fileNames, tot * LEN_FILE * sizeof(char));
    strcpy((LEN_FILE * (tot - 1)) + fileNames, fileName);

    // Store Analisi
    analysis = realloc(analysis, tot * LEN_ANALYSIS * siseof(uint32));
    int temp = LEN_ANALYSIS * (tot - 1);
    for(int i = 0; i < LEN_ANALYSIS; i++)
    {
        analysis[i + temp] = a[i];
    }

    // Store Deleted
    deleted = realloc(deleted, tot * sizeof(int));
    deleted[tot - 1] = 0;

    clearLine(IN);
}

void deleteFile()
{
    char fileName[MAX_PATH_LENGHT];
    readFileName(IN, fileName);

    char *find = strstr(fileNames, fileName);
    int pos = (int) (find - fileNames);

    if(find == NULL)
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