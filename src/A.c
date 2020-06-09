#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define in stdin
#define OUT STDOUT_FILENO

char **filenames;
int filenamesLen = 0;

int P = 3;
int Q = 4;

int WRITE_C = 0;
int READ_C = 0;
int PID_C = 0;

int WRITE_R = 0;

int READ_REPORTER = -1;
bool isReporting = false;

bool startC();
void readCommand();
void addFile(char *f);
void removeFile(char *f);
void printFiles();
bool startAReport();
bool checkFileExist(char *f);
void quit();

void sighandle_int(int sig)
{
    quit();
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sighandle_int);
    if (argc >= 2)
        WRITE_R = atoi(argv[1]);
    else
    {
        char *myfifo = NAMED_PIPE;
        mkfifo(myfifo, 0666);
        WRITE_R = open(myfifo, O_WRONLY);
    }

    if (argc >= 3)
        P = atoi(argv[2]);
    if (argc >= 4)
        Q = atoi(argv[3]);

    if (P <= 0)
        P = 3;
    if (Q <= 0)
        Q = 4;

    if (!startC())
    {
        fprintf(stderr, "Couldn't start anoter process, please try again later");
        exit(ERR_FORK);
    }

    logg("A started");
    while (true)
        readCommand();
    return 0;
}

bool startC()
{
    int fdDOWN[2];
    pipe(fdDOWN);
    WRITE_C = fdDOWN[WRITE];

    int fdUP[2];
    pipe(fdUP);
    READ_C = fdUP[READ];

    pid_t pid = fork();
    if (pid == 0)
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Pstr[9];
        sprintf(Pstr, "%d", P);
        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        execlp(FILENAME_C, FILENAME_C, Pstr, Qstr, (char *)NULL);
        execErrorHandleAndExit(STDOUT_FILENO, fdDOWN[READ], fdUP[WRITE]);
    }
    else if (pid < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        return false;
    }

    PID_C = pid;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);

    if (readSimpleYNResponce(READ_C))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void readCommand()
{
    size_t buffSize = MAX_CMD_LENGHT;
    char *inLine = NULL;
    int len = getline(&inLine, &buffSize, in);
    if (len <= 1)
        return;
    inLine[--len] = '\0';
    char **cmds = NULL;
    char *p = strtok(inLine, " ");
    int num = 0;

    while (p)
    {
        cmds = realloc(cmds, sizeof(char *) * ++num);
        if (cmds == NULL)
        {
            exit(0);
        }
        cmds[num - 1] = p;
        p = strtok(NULL, " ");
    }
    optind = 1; //RESET COSO CHE LETTE OPTs

    if (num > 0)
    {
        if (strcmp(cmds[0], "P") == 0)
        {
            if (num == 3)
            {
                int p = atoi(cmds[1]);
                int q = atoi(cmds[2]);
                if (p > 0 && q > 0)
                {
                    P = p;
                    Q = q;
                    printSuccess(OUT);
                }
                else
                    printFail(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "A") == 0) //ADD FILE
        {
            if (num == 2 && checkFileExist(cmds[1]))
            {
                addFile(cmds[1]);
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "R") == 0) //REMOVE FILE
        {
            if (num == 2 && checkFileExist(cmds[1]))
            {
                removeFile(cmds[1]);
                printSuccess(OUT);
            }
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "L") == 0) //LIST FILE
        {
            printFiles();
        }
        else if (strcmp(cmds[0], "S") == 0) //START REPORT
        {
            if (startAReport())
                printSuccess(OUT);
            else
                printFail(OUT);
        }
        else if (strcmp(cmds[0], "K") == 0) //KILL
        {
            quit();
        }
        else
        {
            printFail(OUT);
        }
    }
    free(inLine);
    free(cmds);
}

void addFile(char *f)
{
    int i;
    for (i = 0; i < filenamesLen; i++)
        if (strcmp(filenames[i], f) == 0)
            return;

    filenames = (char **)realloc(filenames, ++filenamesLen * sizeof(char *));
    filenames[filenamesLen - 1] = malloc(MAX_PATH_LENGHT * sizeof(char));
    strcpy(filenames[filenamesLen - 1], f);
}

void removeFile(char *f)
{
    int found;
    for (found = 0; found < filenamesLen; found++)
        if (strcmp(filenames[found], f) == 0)
            break;

    if (found == filenamesLen)
        return;

    char **temp = (char **)malloc((filenamesLen - 1) * sizeof(char *));

    if (found != 0)
        memcpy(temp, filenames, found * sizeof(char *));
    if (found != (filenamesLen - 1))
        memcpy(temp + found, filenames + found + 1, (filenamesLen - found - 1) * sizeof(char *));

    filenamesLen--;
    free(filenames[found]);
    free(filenames);
    filenames = temp;
}

void printFiles()
{
    int i;
    for (i = 0; i < filenamesLen; i++)
    {
        write(OUT, filenames[i], strlen(filenames[i]));
        write(OUT, "\n", 1);
    }
    write(OUT, "\n", 1);
}

bool checkFileExist(char *f)
{
    bool trovato = false;
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if (pid > 0) //PARENT
    {
        close(fd[WRITE]);
        char buff[MAX_PATH_LENGHT];
        int letti = read(fd[READ], buff, MAX_PATH_LENGHT);
        if (letti == 0)
            trovato = true;
        close(fd[READ]);
    }
    else if (pid == 0) //CHILD
    {
        close(fd[READ]);
        dup2(fd[WRITE], STDERR_FILENO);
        dup2(open("/dev/null", O_WRONLY), STDOUT_FILENO);

        execl("/usr/bin/find", "/usr/bin/find", f, NULL);
        error("EXEC ERROR");
    }
    else
    {
        error("FORK ERROR");
    }
    return trovato;
}

// ----- REPORT STUFF -----
void *sendStuff();
void *readStuff();

int FIND_READ_FILES = -1;
bool startAReport()
{
    if (isReporting)
        return false;
    if (filenamesLen == 0)
        return true;

    int toFindArgLen = filenamesLen + 4;
    char **toFindArg = (char **)malloc(toFindArgLen * sizeof(char *));
    int i;
    for (i = 0; i < filenamesLen; i++)
        toFindArg[i + 1] = filenames[i];
    toFindArg[filenamesLen + 1] = "-type";
    toFindArg[filenamesLen + 2] = "f";
    toFindArg[filenamesLen + 3] = NULL;

    free(filenames);
    filenamesLen = 0;
    filenames = (char **)malloc(0);

    isReporting = true;

    //CREATE PROCESSES
    sendPandQ(WRITE_C, P, Q);
    if (!readSimpleYNResponce(READ_C))
        return false;

    //FIND FILES
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == 0) //CHILD
    {
        close(fd[READ]);
        dup2(fd[WRITE], STDOUT_FILENO);
        dup2(open("/dev/null", O_WRONLY), STDERR_FILENO);
        toFindArg[0] = "/usr/bin/find";
        execv("/usr/bin/find", toFindArg);

        error("FIND NOT FOUND");
        close(fd[WRITE]);
        exit(ERR_EXEC);
    }
    else if (pid < 0)
    {
        error("FORK FIND ERROR");
        close(fd[READ]);
        close(fd[WRITE]);
        return false;
    }

    close(fd[WRITE]);
    FIND_READ_FILES = fd[READ];

    pthread_t thredSender, thredReciver;
    pthread_create(&thredSender, NULL, sendStuff, NULL);
    pthread_create(&thredSender, NULL, readStuff, NULL);

    //CLEAR
    for (i = 1; i < toFindArgLen - 3; i++)
    {
        free(toFindArg[i]);
    }
    free(toFindArg);
    toFindArgLen = 0;

    return true;
}

void *sendStuff()
{

    //START LA COSA CHE MAND INDIETRO
    sendStart(WRITE_C);

    char line[MAX_PATH_LENGHT];
    int letti;
    char c;
    int i = 0;
    while ((letti = read(FIND_READ_FILES, &c, 1)) > 0)
    {
        line[i] = c;
        if (c == '\n')
        {
            line[i] = '\0';
            if (i > 0)
            {
                sendFilename(WRITE_C, line, strlen(line));
            }
            i = 0;
        }
        else
            i++;
    }
    sendFine(WRITE_C);
    close(FIND_READ_FILES);

    isReporting = false;
}

void *readStuff()
{
    sendStart(WRITE_R);

    char cmd = 'F';
    char filename[MAX_PATH_LENGHT];
    while (true)
    {
        read(READ_C, &cmd, 1);
        if (cmd == CMD_END)
        {
            read(READ_C, &cmd, 1); // READ \n
            break;
        }
        else if (cmd == CMD_FILE)
        {
            int filenameLen = readFilename(READ_C, filename);
            Analysis a = readAnalysis(READ_C);
            sendFilename(WRITE_R, filename, filenameLen);
            printAnalysis(WRITE_R, a);
        }
    }
    sendFine(WRITE_R);
}

void quit()
{
    sendKill(WRITE_C);
    logg("A killed");
    exit(0);
}