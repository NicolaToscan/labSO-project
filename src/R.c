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

#define LEN_FILE MAX_PATH_LENGHT
#define LEN_ANALYSIS ANAL_LENGTH

int tot = 0;
char *fileNames_strings;
int *analysis_elems;
int *deleted;


void doReadAnalysis();
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
    int i; for(i = 0; i < LEN_ANALYSIS; i++)
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
    int pos = (int) (find - fileNames_strings);

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