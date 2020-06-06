#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/common.h"
#include "lib/dataprotocol.h"

#define INPUT_FILE STDIN_FILENO
#define OUTPUT_FILE STDOUT_FILENO

#define WRITE_INDEX 0
#define READ_INDEX 1
#define PID_INDEX 2

typedef struct QData_s
{
	int write;
	int read;
	int pid;

} QData;

QData *resizeQs(const int newN, int *nCurr, QData *Q);
void killQ(const int write, const int read);
void resetQ(const int write, const int read, const int section, const int sections);
void startQ(QData *qData, const int section, const int sections);
void forwardFile(int numberOfFiles, QData *Q, int qLen);
void returnResult(QData *Q, int qLen);

int main(int argc, char *argv[])
{
	int sections = 1;
	int nrOfFile;
	int currentQs = 1;
	QData *Q = calloc(currentQs, sizeof(QData));

	startQ(Q, 1, 1);

	while (1)
	{
		char cmd = readFirstChar(INPUT_FILE);
		switch (cmd)
		{

		case COMMAND_FILE:
			nrOfFile = readNumberOfFiles(INPUT_FILE);
			forwardFile(nrOfFile, Q, currentQs);
			returnResult(Q, currentQs);
			break;

		case COMMAND_NR_SECTION:
			sections = readSectionNumber(INPUT_FILE);
			Q = resizeQs(sections, &currentQs, Q);
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

QData *resizeQs(const int newN, int *nCurr, QData *Q)
{
	if (newN == *nCurr)
		return Q;

	QData *newQ = (QData *)calloc(newN, sizeof(QData));

	int toCopy = newN < *nCurr ? newN : *nCurr;
	int i = 0;
	for (i = 0; i < toCopy; i++)
	{
		newQ[i].read = Q[i].read;
		newQ[i].write = Q[i].write;
		newQ[i].pid = Q[i].pid;
		resetQ(newQ[i].read, newQ[i].write, i + 1, newN);
	}

	if (newN > *nCurr)
	{
		for (; i < newN; i++)
			startQ(newQ + i, i + 1, newN);
	}
	else //(n < nCurr)
	{
		for (; i < *nCurr; i++)
			killQ(Q[i].read, Q[i].write);
	}

	free(Q);
	*nCurr = newN;
	return newQ;
}

void killQ(const int WRITE, const int READ)
{
	char buff[2] = {COMMAND_QUIT, '\n'};
	write(WRITE, buff, 2);
	//TODO close pipe
}

void resetQ(const int write, const int read, const int section, const int sections)
{
	if (section > sections)
	{
		error("Index bigger that range");
		errorKillAll(ERR_CO_OUTOFRANGE);
		exit(ERR_CO_OUTOFRANGE);
	}

	sendCharCommand(write, COMMAND_NR_SECTION);
	sendIntCommand(write, sections);
	sendCharCommand(write, COMMAND_SECTION);
	sendIntCommand(write, section);
}

void startQ(QData *qData, const int section, const int sections)
{
	int fdDOWN[2];
	pipe(fdDOWN);
	qData->write = fdDOWN[WRITE];

	int fdUP[2];
	pipe(fdUP);
	qData->read = fdUP[READ];

	pid_t pid = fork();
	if (pid > 0) //Parent
	{
		qData->pid = pid;
		close(fdDOWN[READ]);
		close(fdUP[WRITE]);
		resetQ(qData->write, qData->read, section, sections);
	}
	else if (pid == 0) //Child
	{
		close(fdDOWN[WRITE]);
		close(fdUP[READ]);

		dup2(fdDOWN[READ], STDIN_FILENO);
		dup2(fdUP[WRITE], STDOUT_FILENO);

		//close(fdDOWN[READ]);
		//close(fdUP[WRITE]);

		if (execlp(FILENAME_Q, FILENAME_Q, (char *)NULL) < 0)
		{
			//TODO: handle exec error
			error("EXEC error");
		}
	}
	else
	{
		//TODO: handle fork error
		error("FORK error");
	}
}

void forwardFile(int numberOfFiles, QData *Q, int qLen)
{
	char fileName[MAX_FILENAME_LENGHT];

	int j = 0;
	for (j = 0; j < qLen; j++)
	{
		sendCommand(Q[j].write, "F");
		sendCommand(Q[j].write, "1"); //TODO fix
		//sendIntCommand(Q[j].write, numberOfFiles);
	}

	int i = 0;
	for (i = 0; i < numberOfFiles; i++)
	{
		int nr = readFileName(INPUT_FILE, fileName, MAX_FILENAME_LENGHT);
		if (nr < 0)
			continue;

		int j = 0;
		for (j = 0; j < qLen; j++)
		{
			sendCommand(Q[j].write, fileName);
		}
	}
}

void returnResult(QData *Q, int qLen)
{
	Analysis a = initAnalysis();
	int i = 0;
	for (i = 0; i < qLen; i++) {
		char buf[100];
		readline(Q[i].read, buf, 100);
		Analysis b = readAnalysis(Q[i].read);
		sumAnalysis(a, b);
	}

	printAnalysis(a);
}