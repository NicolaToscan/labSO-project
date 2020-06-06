#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void printSuccess(const int fd);
void printFail(const int fd);
bool readSimpleYNResponce(const int fd);
void sendKill(const int fd);
void sendQnumbers(const int fd, const int section, const int sections);
void readQnumbers(const int fd, int *section, int *sections);
void sendFilename(const int fd, char *filename, int len);
int readFilename(const int fd, char *filename);
void sendPQs(const int fd, const int qs);
int readPQs(const int fd);
void sendPandQ(const int fd, const int P, const int Q);
void readPandQ(const int fd, int *P, int *Q);

