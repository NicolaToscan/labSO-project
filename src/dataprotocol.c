#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

int readNumberOfFiles(int file)
{
	int N = readNumber(file);
	if (N <= 0)
	{
		error("Number not valid");
		return 0;
	}
	return N;
}

int readSectionNumber(int file)
{
	int M = readNumber(file);
	if (M <= 0)
	{
		error("Number not valid");
		return 1;
	}

	return M;
}

int readMySection(int file, int max)
{
	int N = readNumber(file);
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

int readFileName(int file, char *buffer, int lenght)
{
	int nr = readline(file, buffer, lenght);
	if (nr < 0)
	{
		error("Filename too long");
	}
	return nr;
}