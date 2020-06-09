#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define in stdin
#define out stdout

int WRITE_A = 0;
int READ_A = 0;
int PID_A = 0;

int WRITE_R = 0;
int READ_R = 0;
int PID_R = 0;

int P = 3;
int Q = 4;

void startAandR();
void readCommand();
void file(int argc, char *argv[]);
bool setCmd(int argc, char *argv[], bool sendCmd);
void quit();
void help(int argc, char *argv[]);
bool forwardFile(char type, char *filename);
void doReport();
void reportCmd(int argc, char *argv[]);
bool handleBusyAndResponseReport();
bool removeFileFromReport();
void stampaReport();
void handleArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    logg("M started");

    if (argc > 1)
    {
        handleArgs(argc, argv);
    }
    else
    {
        startAandR();
    }

    while (true)
    {
        printf("# ");
        readCommand();
    }

    return 0;
}

void startAandR()
{
    int pipeAR[2];
    if (pipe(pipeAR) == -1)
    {
        error("Error opening pipe");
        exit(ERR_PIPE);
    }

    int fdDOWN[2];
    int fdUP[2];

    // A
    if (pipe(fdDOWN) == -1 || pipe(fdUP) == -1)
    {
        error("Error opening pipe");
        exit(ERR_PIPE);
    }

    WRITE_A = fdDOWN[WRITE];
    READ_A = fdUP[READ];

    pid_t pidA = fork();

    if (pidA == 0)
    {
        close(pipeAR[READ]);
        char fdWriteToR[9];
        sprintf(fdWriteToR, "%d", pipeAR[WRITE]);

        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        char Pstr[9];
        sprintf(Pstr, "%d", P);
        char Qstr[9];
        sprintf(Qstr, "%d", Q);

        execlp(FILENAME_A, FILENAME_A, fdWriteToR, Pstr, Qstr, (char *)NULL);

        error("COULDN'T START A");
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);
        exit(ERR_EXEC);
    }
    else if (pidA < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        exit(ERR_FORK);
    }
    close(pipeAR[WRITE]);

    PID_A = pidA;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);

    // R
    if (pipe(fdDOWN) == -1 || pipe(fdUP) == -1)
    {
        error("Error opening pipe");
        exit(ERR_PIPE);
    }

    WRITE_R = fdDOWN[WRITE];
    READ_R = fdUP[READ];

    pid_t pidR = fork();

    if (pidR == 0)
    {
        char fdReadToR[9];
        sprintf(fdReadToR, "%d", pipeAR[READ]);

        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        execlp(FILENAME_R, FILENAME_R, fdReadToR, (char *)NULL);

        error("COULDN'T START R");
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);
        exit(ERR_EXEC);
    }
    else if (pidR < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        exit(ERR_FORK);
    }
    close(pipeAR[READ]);

    PID_A = pidA;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);
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
        cmds[num - 1] = p;
        p = strtok(NULL, " ");
    }

    optind = 0;
    if (num > 0)
    {
        if (strcmp(cmds[0], "set") == 0)
        {
            setCmd(num, cmds, true);
        }
        else if (strcmp(cmds[0], "file") == 0)
        {
            file(num, cmds);
        }
        else if (strcmp(cmds[0], "report") == 0)
        {
            reportCmd(num, cmds);
        }
        else if (strcmp(cmds[0], "help") == 0)
        {
            help(num, cmds);
        }
        else if (strcmp(cmds[0], "q") == 0 || strcmp(cmds[0], "quit") == 0 || strcmp(cmds[0], "exit") == 0)
        {
            sendKill(WRITE_A);
            sendKill(WRITE_R);
            quit();
        }
        else
        {
            printf("Command '%s' not found, type help\n", cmds[0]);
        }
    }

    free(inLine);
    free(cmds);
}

// P and Q set

bool setCmd(int argc, char *argv[], bool sendCmd)
{

    bool pSet = false, qSet = false;
    int p = 0, q = 0;
    char c;
    while ((c = getopt(argc, argv, "p:q:")) != -1)
    {
        switch (c)
        {
        case 'p':
            pSet = true;
            p = atoi(optarg);
            break;
        case 'q':
            qSet = true;
            q = atoi(optarg);
            break;

        case '?':
            if (optopt == 'q')
                printf("set: argument for 'q' not found\n");
            else if (optopt == 'p')
                printf("set: argument for 'p' not found\n");

            break;

        default:
            break;
        }
    }

    if (pSet)
    {
        if (p <= 0)
            printf("set: argument for 'p' not valid, usign previus value\n");
        else
            P = p;
    }

    if (qSet)
    {
        if (q <= 0)
            printf("set: argument for 'q' not valid, usign previus value\n");
        else
            Q = q;
    }

    char cmd[32];
    sprintf(cmd, "P %d %d\n", P, Q);
    write(WRITE_A, cmd, strlen(cmd));
    if (readSimpleYNResponce(READ_A))
    {
        printf("Values updated successfully\n");
        return true;
    }
    else
    {
        printf("Couldn't update values\n");
        return false;
    }
}

// FILE handler

void file(int argc, char *argv[])
{
    char c;
    while ((c = getopt(argc, argv, "la:r:")) != -1)
    {
        switch (c)
        {
            //ADD FILE
        case 'a':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
                forwardFile('A', argv[optind]);
            break;

            //REMOVE FILE
        case 'r':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
                forwardFile('R', argv[optind]);
            break;

            //LIST
        case 'l':
            write(WRITE_A, "L\n", 2);
            char line[MAX_PATH_LENGHT];
            int rd;
            do
            {
                rd = readline(READ_A, line, MAX_PATH_LENGHT);
                printf("%s\n", line);
            } while (rd != 0);
            break;

        case '?':
            if (optopt == 'a')
                printf("set: argument for 'a' not found\n");
            else if (optopt == 'r')
                printf("set: argument for 'r' not found\n");
            break;

        default:
            break;
        }
    }
}

bool forwardFile(char type, char *filename)
{
    char cmd[2] = {type, ' '};
    write(WRITE_A, cmd, 2);
    write(WRITE_A, filename, strlen(filename));
    write(WRITE_A, "\n", 1);

    bool res = readSimpleYNResponce(READ_A);
    if (!res)
        printf("File or directory '%s' not found\n", filename);
    return res;
}

// REPORT cmdsd

void reportCmd(int argc, char *argv[])
{
    char c;
    bool fatto = false;
    while ((c = getopt(argc, argv, "sclr:")) != -1)
    {
        if (!fatto)
        {
            switch (c)
            {
            case 's': //START
                doReport();
                break;

            case 'c': //CLEAN
                sendCharCommand(WRITE_R, CMD_CLEAN);
                if (handleBusyAndResponseReport())
                    printf("Report cleaned\n");
                else
                    printf("Couldn't clean report\n");
                break;

            case 'l': //SHOW
                sendCharCommand(WRITE_R, CMD_REQUEST_REPORT);
                stampaReport();
                break;

            case 'r': //REMOVE
                optind--;
                bool bloccato = false;
                for (; optind < argc && *argv[optind] != '-'; optind++)
                    if (!bloccato && !removeFileFromReport(argv[optind]))
                        bloccato = true;
                break;

            case '?':
                if (optopt == 'r')
                    printf("set: argument for 'r' not found\n");

                break;

            default:
                break;
            }
            fatto = true;
        }
    }
}

void stampaReport()
{
    char cmd = readchar(READ_R);
    if (cmd == CMD_BUSY)
    {
        char nFile[12];
        readline(READ_R, nFile, 12);
        printf("Report is still working, %s files done\n", nFile);
        return;
    }

    cmd = readchar(READ_R); // READ \n

    char line[MAX_PATH_LENGHT];
    int rd;
    do
    {
        rd = readline(READ_R, line, MAX_PATH_LENGHT);
        printf("%s\n", line);
    } while (rd != 0);
}

bool handleBusyAndResponseReport()
{
    char cmd = readchar(READ_R);
    if (cmd == CMD_BUSY)
    {
        char nFile[12];
        readline(READ_R, nFile, 12);
        printf("Report is still working, %s files done\n", nFile);
        return false;
    }
    else
    {
        clearLine(READ_R);
        return cmd == RESPONSE_OK ? true : false;
    }
}

bool removeFileFromReport(char *filename)
{
    char cc = CMD_REMOVE_FILE;
    write(WRITE_R, &cc, 1);
    write(WRITE_R, filename, strlen(filename));
    write(WRITE_R, "\n", 1);

    char cmd = readchar(READ_R);
    if (cmd == CMD_BUSY)
    {
        char nFile[12];
        readline(READ_R, nFile, 12);
        printf("Report is still working, %s files done\n", nFile);
        return false;
    }
    else
    {
        clearLine(READ_R);
        return true;
    }
}

void doReport()
{
    write(WRITE_A, "S\n", strlen("S\n"));
    if (readSimpleYNResponce(READ_A))
        printf("Report started in background\n");
    else
        printf("Couldn't start report\n");
}

// ARGS

void handleArgs(int argc, char *argv[])
{

    bool err = false;

    optind = 0;

    int p, q;
    char c;
    while ((c = getopt(argc, argv, "p:q:f:")) != -1)
    {
        switch (c)
        {
        case 'f':
            break;
        case 'p':
            p = atoi(optarg);
            break;
        case 'q':
            q = atoi(optarg);
            break;
        case '?':
            if (optopt == 'f')
            {
                err = true;
                printf("set: argument for 'f' not found\n");
            }
            else if (optopt == 'p')
            {
                err = true;
                printf("set: argument for 'p' not found\n");
            }
            else if (optopt == 'q')
            {
                err = true;
                printf("set: argument for 'q' not found\n");
            }
            break;

        default:
            break;
        }
    }
    if (err)
        exit(ERR_INVALID_NR);

    if (p > 0)
        P = p;
    if (Q > q)
        Q = q;

    startAandR();

    optind = 0;
    while ((c = getopt(argc, argv, "p:q:f:")) != -1)
    {
        switch (c)
        {
            //ADD FILE
        case 'f':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
                forwardFile('A', argv[optind]);
            break;
        case 'p':
            break;
        case 'q':
            break;
        case '?':
            if (optopt == 'f')
            {
                err = true;
                printf("set: argument for 'f' not found\n");
            }
            break;

        default:
            break;
        }
    }

    if (err)
        quit();

    doReport();
    sendCharCommand(WRITE_R, CMD_REQUEST_REPORT_WHEN_READDY);
    stampaReport();
}

// Other

void help(int argc, char *argv[])
{
    printf("\n");
    printf(" set: \n");
    printf("  set [-p numberofP] [-q numberofQ] \n");
    printf("  edit p and q \n");
    printf("  options:\n");
    printf("    -p numberofP edit the number of P \n");
    printf("    -q numberofQ edit the number of Q \n\n");
    printf(" file: \n");
    printf("  file [-a filepath] [-r filepath] [-u filepath] [-l] \n");
    printf("  manage files\n");
    printf("  options:\n");
    printf("   -a filepath add a file \n");
    printf("   -r filepath remove a file \n");
    printf("   -u filepath recheck file\n");
    printf("   -l print the added files \n\n");
    printf(" report: \n");
    printf("  report\n");
    printf("  starts the report \n\n");
    printf(" quit: \n");
    printf("  quit \n");
    printf("  kill all the process and quit \n");
}

void quit()
{
    logg("M killed");
    exit(0);
}