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
void quit();

int main(int argc, char *argv[])
{
	if (argc >= 3)
	{
		mySection = atoi(argv[1]);
		totSections = atoi(argv[2]);
	}
	else
	{
		printFail(OUT);
		exit(ERR_INVALID_NR);
	}

	if (mySection <= 0 || totSections <= 0)
	{
		printFail(OUT);
		exit(ERR_INVALID_NR);
	}

	logg("Q started");
	printSuccess(OUT);

	while (true)
	{
		char cmd;
		int r = read(IN, &cmd, 1);
		if (r == 0)
			quit();

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
			quit();
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

void quit()
{
	logg("Q KILLED");
	exit(0);
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

	Analysis a = analyseFile(myFile, mySection, totSections);
	printAnalysis(OUT, a);
}
