#include <unistd.h>
#include <stdio.h>
#include "common.h"

int readline(const int file, char *buffer, const int maxsize)
{
	int i = 0;
	while (i < maxsize)
	{
		char CH[1];

		int rd = read(file, CH, 1);
		buffer[i] = CH[0];
		if (CH[0] == '\n')
		{
			buffer[i] = '\0';
			return i;
		}
		i++;
	}
	buffer[maxsize - 1] = '\0';
	return -1;
}

char readFirstChar(const int file)
{
	char CH[2];
	readline(file, CH, 2);
	return CH[0];
}

int readNumber(int file)
{
	char number[10];
	if (readline(file, number, 10) == -1)
	{
		error("Input too big");
		return 1;
	}

	return atoi(number);
}

void error(char msg[])
{
	fprintf(stderr, "\033[31;1m ERROR: %s \033[0m\n", msg);
}

Analysis initAnalysis()
{
	Analysis a;
	a.letters = 0;
	a.numbers = 0;
	a.other = 0;
	a.punctuaction = 0;
	a.spaces = 0;
	a.tot = 0;
	return a;
}

void printAnalysis(Analysis *a)
{
	fprintf(stdout, "%d %d %d %d %d %d\n",
			a->letters,
			a->numbers,
			a->other,
			a->punctuaction,
			a->spaces,
			a->tot);

}

Analysis readAnalysis(int file)
{
	char buffer[60];
	readline(file, buffer, 60);

	Analysis a;
	char *pEnd;
	a.letters = strtol(buffer, &pEnd, 10);
	a.numbers = strtol(pEnd, &pEnd, 10);
	a.other = strtol(pEnd, &pEnd, 10);
	a.punctuaction = strtol(pEnd, &pEnd, 10);
	a.spaces = strtol(pEnd, &pEnd, 10);
	a.punctuaction = strtol(pEnd, NULL, 10);

	return a;
}