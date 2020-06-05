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

int mySection = 1;
int nrOfSections = 1;
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
			readNumbers();
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
			logg("CMD NOT FOUND");
			break;
		}
	}

	return 0;
}

void readNumbers()
{
	readQnumbers(IN, &mySection, &nrOfSections);
	clearLine(IN);
}

void doFile()
{
	char filename[4096];
	readFilename(IN, filename);
	clearLine(IN);
}

void analyseFile(char *f)
{
	
}