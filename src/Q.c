#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"

#define IN STDIN_FILENO
#define OUT STDOUT_FILENO

int mySection = 1;
int nrOfSections = 1;
void readNumbers();

int main(int argc, char *argv[])
{
	logg("Q started");

	while (true)
	{
		char cmd = readchar(IN);

		switch (cmd)
		{

			//NUMBERS
		case Q_CMD_NUMBERS:
			readNumbers();
			break;

			//FILE
		case Q_CMD_FILE:
			readNumbers();
			break;

			//KILL
		case Q_CMD_KILL:
			clearLine(IN);
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
	int n[2];
	read(IN, n, 2 * sizeof(int));
	mySection = n[0];
	nrOfSections = n[1];
	clearLine(IN);

	logg("FATTO");
	loggN(mySection);
	loggN(nrOfSections);
}

void doFile()
{
	int len;
	read(IN, &len, sizeof(int));
	char *filename = (char *)malloc(len * sizeof(char));
	read(IN, filename, len * sizeof(char));
	clearLine(IN);
}