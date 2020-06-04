#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

void errorKillAll(const int errCode)
{
}

void error(char msg[])
{
	fprintf(stderr, "\033[31;1m ERROR: %s \033[0m\n", msg);
}

void logg(char msg[])
{
	fprintf(stderr, "\033[32;1m LOG: %s \033[0m\n", msg);
}

void loggN(int n)
{
	fprintf(stderr, "\033[32;1m LOG: %d \033[0m\n", n);
}

int readline(const int file, char *buffer, const int maxsize)
{
	int i = 0;
	while (i < maxsize)
	{
		char c;

		int rd = read(file, &c, 1);
		buffer[i] = c;
		if (c == '\n')
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

int readNumber(const int file)
{
	char number[10];
	if (readline(file, number, 10) == -1)
	{
		error("Input too big");
		return 1;
	}

	return atoi(number);
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

void printAnalysis(Analysis a)
{
	fprintf(stdout, "%d %d %d %d %d %d\n",
			a.letters,
			a.numbers,
			a.other,
			a.punctuaction,
			a.spaces,
			a.tot);
}

Analysis readAnalysis(const int file)
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

Analysis sumAnalysis(Analysis a, Analysis b)
{
	a.letters += b.letters;
	a.numbers += b.numbers;
	a.other += b.other;
	a.punctuaction += b.punctuaction;
	a.spaces += b.spaces;
	a.tot += b.tot;
	return a;
}

void sendCommand(const int file, char *cmd)
{
	write(file, cmd, strlen(cmd));
	write(file, "\n", strlen("\n"));
}
void sendIntCommand(const int file, const int cmd)
{
	char buff[11];
	sprintf(buff, "%d\n", cmd);
	write(file, buff, strlen(buff));
}
void sendCharCommand(const int file, const char cmd)
{
	char buff[3] = {cmd, '\n', '\0'};
	write(file, buff, strlen(buff));
}