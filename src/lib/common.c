#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

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

// ----- SEND CMD -----
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