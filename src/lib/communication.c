#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "analisys.h"
#include "common.h"
#include "commands.h"

void printSuccess(const int fd)
{
    write(fd, "Y\n", 2 * sizeof(char));
}
void printFail(const int fd)
{
    write(fd, "N\n", 2 * sizeof(char));
}

bool readSimpleYNResponce(const int fd)
{
    char res[2];
    int letti = read(fd, res, 2);
    if (letti != 2)
        return false;

    if (res[0] != RESPONSE_OK)
        return false;

    return true;
}

void sendKill(const int fd)
{
    char cmd[2] = {CMD_KILL, '\n'};
    write(fd, &cmd, 2 * sizeof(char));
}

void sendFilename(const int fd, char *filename, int len)
{
    char cmd = CMD_FILE;
    write(fd, &cmd, sizeof(char));
    write(fd, &len, sizeof(int));
    write(fd, filename, len * sizeof(char));
    write(fd, "\n", 1 * sizeof(char));
}
int readFilename(const int fd, char *filename)
{
    int len;
    read(fd, &len, sizeof(int));
    read(fd, filename, len * sizeof(char));
    filename[len] = '\0';
    return len;
}

void sendQnumbers(const int fd, const int section, const int sections)
{
    char cmd = CMD_Q_NUMBERS;
    write(fd, &cmd, sizeof(char));
    write(fd, &section, sizeof(int));
    write(fd, &sections, sizeof(int));
    write(fd, "\n", sizeof(char));
}
void readQnumbers(const int fd, int *section, int *sections)
{
    int n[2];
    read(fd, n, 2 * sizeof(int));
    *section = n[0];
    *sections = n[1];
}

void sendPQs(const int fd, const int qs)
{
    char cmd = CMD_P_Qs;
    write(fd, &cmd, sizeof(char));
    write(fd, &qs, sizeof(int));
    write(fd, "\n", sizeof(char));
}
int readPQs(const int fd)
{
    int q;
    read(fd, &q, sizeof(int));
    return q;
}

void sendPandQ(const int fd, const int P, const int Q)
{
    char cmd = CMD_C_PandQ;
    write(fd, &cmd, sizeof(char));
    write(fd, &P, sizeof(int));
    write(fd, &Q, sizeof(int));
    write(fd, "\n", sizeof(char));
}
void readPandQ(const int fd, int *P, int *Q)
{
    read(fd, P, sizeof(int));
    read(fd, Q, sizeof(int));
}