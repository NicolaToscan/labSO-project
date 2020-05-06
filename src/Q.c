#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "dataprotocol.h"

#define INPUT_FILE STDIN_FILENO
#define OUTPUT_FILE STDOUT_FILENO

void analyseFiles(int numberOfFiles, Analysis *a);
void analyseChar(char c, Analysis *a);

int main(int argc, char *argv[])
{
	int sections = 1;
	int mySection = 1;
	int nrOfFile;

	while (1)
	{
		char cmd = readFirstChar(INPUT_FILE);
		switch (cmd)
		{

		case COMMAND_FILE:
			nrOfFile = readNumberOfFiles(INPUT_FILE);
			Analysis a = initAnalysis();
			analyseFiles(nrOfFile, &a);
			printAnalysis(&a);
			break;

		case COMMAND_NR_SECTION:
			sections = readSectionNumber(INPUT_FILE);
			break;

		case COMMAND_SECTION:
			mySection = readMySection(INPUT_FILE, sections);
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

void analyseChar(char c, Analysis *a)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		a->letters++;
	else if (c >= '1' && c <= '9')
		a->numbers++;
	else if (c == ' ')
		a->spaces++;
	else if (c == '.' || c == ',' || c == ':' || c == ';' || c == '?' || c == '!')
		a->punctuaction++;
	else
		a->other++;

	a->tot++;
}

void analyseFiles(int numberOfFiles, Analysis *a)
{
	char fileName[MAX_FILENAME_LENGHT];
	char buffer[128];

	int i = 0;
	for (i = 0; i < numberOfFiles; i++)
	{
		int nr = readFileName(INPUT_FILE, fileName, MAX_FILENAME_LENGHT);
		if (nr < 0)
			continue;

		int fd = open(fileName, O_RDONLY);
		if (fd < 0)
		{
			error("Impossible to open");
			continue;
		}

		int rd;
		while (rd = read(fd, buffer, 127))
		{
			if (rd > 0)
				buffer[rd] = '\0';

			int j = 0;
			for (j = 0; j < rd; j++)
			{
				analyseChar(buffer[j], a);
			}
		}

		close(fd);
	}

	logg("File analysed");
}
