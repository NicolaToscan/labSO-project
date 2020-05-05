#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "dataprotocol.h"

#define INPUT_FILE STDIN_FILENO
#define OUTPUT_FILE STDOUT_FILENO

int main(int argc, char *argv[])
{
	int sections = 1;
	int nrOfFile;

	while (1)
	{
		char cmd = readFirstChar(INPUT_FILE);
		switch (cmd)
		{

		case COMMAND_FILE:
			nrOfFile = readNumberOfFiles(INPUT_FILE);
			break;

		case COMMAND_NR_SECTION:
			sections = readSectionNumber(INPUT_FILE);
			break;

		case COMMAND_QUIT:
			exit(0);
			break;

		default:
			error("Unknown command");
			break;
		}
	}

	return 0;
}