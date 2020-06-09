#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
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

int P = DEFAULT_P;
int Q = DEFAULT_Q;

void startAandR();
void readCommand();
void file(int argc, char *argv[]);
bool setCmd(int argc, char *argv[], bool sendCmd);
void quit();
void help();
void helpInline();
bool forwardFile(char type, char *filename);
void doReport();
void reportCmd(int argc, char *argv[]);
bool handleBusyAndResponseReport();
bool removeFileFromReport();
void stampaReport();
void handleArgs(int argc, char *argv[]);

void sighandle_int(int sig)
{
    quit();
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sighandle_int);

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

        char execFilenam[MAX_PATH_LENGHT];
        getExecFilename(FILENAME_A, execFilenam);
        execlp(execFilenam, execFilenam, fdWriteToR, Pstr, Qstr, (char *)NULL);

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

        char execFilenam[MAX_PATH_LENGHT];
        getExecFilename(FILENAME_R, execFilenam);
        execlp(execFilenam, execFilenam, fdReadToR, (char *)NULL);

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
            help();
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
        if (pSet || qSet)
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
    while ((c = getopt(argc, argv, "scmlwr:")) != -1)
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

            case 'm': //SHOW MINIMAL
                sendCharCommand(WRITE_R, CMD_REQUEST_REPORT_MINIMAL);
                stampaReport();
                break;

            case 'w': //SHOW SUBITO
                doReport();
                sendCharCommand(WRITE_R, CMD_REQUEST_REPORT_WHEN_READDY);
                stampaReport();
                break;

            case 'r': //REMOVE
                optind--;
                bool bloccato = false;
                for (; optind < argc && *argv[optind] != '-'; optind++)
                    if (!bloccato && !removeFileFromReport(argv[optind]))
                        bloccato = true;
                if (!bloccato)
                    printf("File removed\n");
                else
                    printf("Some file couldn't be removed\n");

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
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0)
    {
        helpInline();
        exit(0);
    }

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

void helpInline()
{
    printf("usage: M.out [-p <number>] [-q <number>] [-f <filename1> <filename2> ...] \n");
    printf("   options:\n");
    printf("     -p edit the number of P \n");
    printf("     -q edit the number of Q \n");
    printf("     -f list of file and directory to report \n");
    printf("\n");
}

void help()
{
    printf("\n");
    printf("set [-p <number>] [-q <number>] \n");
    printf("   set the number of Ps and Qs \n");
    printf("   options:\n");
    printf("     -p edit the number of P \n");
    printf("     -q edit the number of Q \n");
    printf("\n");

    printf("file [-a <filename1> <filename2> ...] [-r <filename1> <filename2> ...] \n");
    printf("   add and remove files and directory for new report \n");
    printf("   options:\n");
    printf("     -a list of filenames to add \n");
    printf("     -r list of filenames to remove \n");
    printf("\n");

    printf("file -l\n");
    printf("   list all file and directory that are currently wait to be reported \n");
    printf("\n");

    printf("report -s\n");
    printf("   start a new report in background \n");
    printf("\n");

    printf("report -l\n");
    printf("   show results of a report \n");
    printf("\n");

    printf("report -w\n");
    printf("   start a new report and show its results when it's finished \n");
    printf("\n");

    printf("report -m\n");
    printf("   show a summary of the result of a report \n");
    printf("\n");

    printf("report -r <filename1> <filename2> ... \n");
    printf("   remove files from current report \n");
    printf("   options:\n");
    printf("     -r list of files to remove \n");
    printf("\n");

    printf("report -c\n");
    printf("   clean current report \n");
    printf("\n");

    printf("quit | exit | q\n");
    printf("   close all processes and exit \n");
    printf("\n");

    printf("\n");
}

void quit()
{
    exit(0);
}