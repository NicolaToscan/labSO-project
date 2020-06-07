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

int mySection;
int totSections;

void readNumbers();
void doFile();

int main(int argc, char *argv[])
{
	if (argc >= 3)
	{
		mySection = atoi(argv[1]);
		totSections = atoi(argv[2]);
	}
	else
	{
		//TODO: oh no
	}

	if (mySection <= 0 || totSections <= 0)
	{
		//TODO: oh no
	}

	logg("Q started");

	while (true)
	{
		char cmd = readchar(IN);

		switch (cmd)
		{

			//NUMBERS
		case CMD_Q_NUMBERS:
			readNumbers();
			break;

			//FILE
		case CMD_FILE:
			doFile();
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
			logg("CMD NOT FOUND DA Q");
			break;
		}
	}

	return 0;
}

void readNumbers()
{
	readQnumbers(IN, &mySection, &totSections);
	if (mySection <= 0 || totSections <= 0)
	{
		//TODO: oh no
	}
	clearLine(IN);
}

void doFile()
{
	char myFile[MAX_PATH_LENGHT];
	readFilename(IN, myFile);
	clearLine(IN);

	fprintf(stderr, "%d) Ricevuto un file '%s'\n", mySection, myFile);
	
	Analysis a = analyseFile(myFile, mySection, totSections);
	printAnalysisReadable(a);
	//printAnalysis(STDERR_FILENO, a);
}
