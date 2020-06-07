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
	clearLine(IN);
}

void doFile()
{
	char myFile[MAX_PATH_LENGHT];
	readFilename(IN, myFile);
	clearLine(IN);

	Analysis a = analyseFile(myFile, mySection, totSections);
	printAnalysis(OUT, a);
}
