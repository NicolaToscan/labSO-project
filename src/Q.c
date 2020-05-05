#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define INPUT_FILE STDIN_FILENO
#define OUTPUT_FILE STDOUT_FILENO

int readSectionNumber();
int readMySection(int max);
int readNumberOfFiles();
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

		case Q_FILE_COMMAND:
			nrOfFile = readNumberOfFiles();
			Analysis a = initAnalysis();
			analyseFiles(nrOfFile, &a);
			printAnalysis(&a);
			break;

		case Q_NUMBER_OF_SECTIONS_COMMAND:
			sections = readSectionNumber();
			break;

		case Q_MY_SECTION_COMMAND:
			mySection = readMySection(sections);
			break;

		case Q_QUIT_COMMAND:
			exit(0);
			break;

		default:
			error("Unknown command");
			break;
		}
	}

	return 0;
}

int readSectionNumber()
{
	int M = readNumber(INPUT_FILE);
	if (M <= 0)
	{
		error("Number not valid");
		return 1;
	}

	return M;
}

int readMySection(int max)
{
	int N = readNumber(INPUT_FILE);
	if (N <= 0)
	{
		error("Number not valid");
		return 1;
	}

	if (N > max)
	{
		error("Number out of range");
		return 1;
	}

	return N;
}

int readNumberOfFiles()
{
	int N = readNumber(INPUT_FILE);
	if (N <= 0) {
		error("Number not valid");
		return 0;
	}
	return N;
}

void analyseFiles(int numberOfFiles, Analysis *a)
{
	char fileName[MAX_FILENAME_LENGHT];
	char buffer[128];

	int i = 0;
	for (i = 0; i < numberOfFiles; i++)
	{
		int nr = readline(INPUT_FILE, fileName, MAX_FILENAME_LENGHT);
		if (nr < 0)
		{
			error("Filename too long");
			continue;
		}

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
	}
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