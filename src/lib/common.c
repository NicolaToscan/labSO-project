#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "communication.h"

// ----- PRINT -----
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
void loggC(char c)
{
	fprintf(stderr, "\033[32;1m LOG: %c \033[0m\n", c);
}

void clearLine(const int file)
{
	char c;
	do
	{
		int n = read(file, &c, 1);
	} while (c != '\n');
}
char readchar(const int file)
{
	char c;
	int r = read(file, &c, 1);
	return c;
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

void execErrorHandleAndExit(int out, int pipeToCloseA, int pipeToCloseB)
{
	printFail(out);

	if (pipeToCloseA > 0)
		close(pipeToCloseA);
	if (pipeToCloseB > 0)
		close(pipeToCloseB);

	error("EXEC error");
	exit(ERR_EXEC);
}

void forkErrorHandle(int pA, int pB, int pC, int pD)
{
	close(pA);
	close(pB);
	close(pC);
	close(pD);
	error("FORK error");
}