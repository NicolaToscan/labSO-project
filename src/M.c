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

bool startAandR();
void readCommand();
void file(int argc, char *argv[]);
void setCmd(int argc, char *argv[]);
void quit(int argc, char *argv[]);
void help(int argc, char *argv[]);
bool forwardFile(char type, char *filename);
void doReport();
void reportCmd(int argc, char *argv[]);
bool handleBusyAndResponseReport();

int main(int argc, char *argv[])
{
    logg("M started");
    startAandR();

    while (true)
    {
        printf("# ");
        readCommand();
    }

    return 0;
}

bool startAandR()
{
    int pipeAR[2];
    pipe(pipeAR);

    int fdDOWN[2];
    int fdUP[2];

    // A
    pipe(fdDOWN);
    pipe(fdUP);

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

        execlp(FILENAME_A, FILENAME_A, fdWriteToR, (char *)NULL);
        //TODO: speriamo bene

        error("COULDN'T START A");
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);
        exit(ERR_EXEC);
    }
    else if (pidA < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        return false;
    }
    close(pipeAR[WRITE]);

    PID_A = pidA;
    close(fdDOWN[READ]);
    close(fdUP[WRITE]);

    // R
    pipe(fdDOWN);
    pipe(fdUP);

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
        //TODO: speriamo bene

        error("COULDN'T START R");
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);
        exit(ERR_EXEC);
    }
    else if (pidR < 0)
    {
        forkErrorHandle(fdDOWN[READ], fdDOWN[WRITE], fdUP[READ], fdUP[WRITE]);
        return false;
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
        if (cmds == NULL)
        {
            //TODO: MERDA
            exit(0);
        }
        cmds[num - 1] = p;
        p = strtok(NULL, " ");
    }
    optind = 1; //RESET COSO CHE LETTE OPTs
    if (num > 0)
    {
        if (strcmp(cmds[0], "set") == 0)
        {
            setCmd(num, cmds);
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
        else if (strcmp(cmds[0], "quit") == 0 || strcmp(cmds[0], "q") == 0)
        {
            sendKill(WRITE_A);
            sendKill(WRITE_R);
            quit(num, cmds);
        }
        else
        {
            printf("Command '%s' not found, type help\n", cmds[0]);
        }
    }

    free(inLine);
    free(cmds);
}

void setCmd(int argc, char *argv[])
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
            printf("set: argument for 'n' not valid\n");
        else
            P = p;
    }

    if (qSet)
    {
        if (q <= 0)
            printf("set: argument for 'm' not valid\n");
        else
            Q = q;
    }

    char cmd[32];
    sprintf(cmd, "P %d %d\n", P, Q);
    write(WRITE_A, cmd, strlen(cmd));
    if (readSimpleYNResponce(READ_A))
        printf("Values updated successfully\n");
    else
        printf("Couldn't update values\n");
}

void file(int argc, char *argv[])
{
    char c;
    while ((c = getopt(argc, argv, "la:r:u:")) != -1)
    {
        switch (c)
        {
            //ADD FILE
        case 'a':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                forwardFile('A', argv[optind]);
            }
            break;

            //REMOVE FILE
        case 'r':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                forwardFile('R', argv[optind]);
            }
            break;

            //RECHECK FILE
        case 'u':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                forwardFile('U', argv[optind]);
            }
            break;

            //LIST
        case 'l':
            write(WRITE_A, "L\n", 2);
            int letti;
            char buff[64];
            int lastChar = '\0';
            while ((letti = read(READ_A, buff, 64)) > 0)
            {
                lastChar = buff[letti - 1];
                if (letti != 64)
                    buff[letti] = '\0';

                printf("%s", buff);

                if (letti > 2) // TODO: sistema sta merda
                {
                    if (buff[letti - 1] == '\n' && buff[letti - 2] == '\n')
                        break;
                }
                else if (letti == 1)
                {
                    if (buff[letti - 1] == '\n' && lastChar == '\n')
                        break;
                }
                else
                    break;
            }
            break;

        case '?':
            if (optopt == 'a')
                printf("set: argument for 'm' not found\n");
            else if (optopt == 'r')
                printf("set: argument for 'n' not found\n");
            else if (optopt == 'w')
                printf("set: argument for 'u' not found\n");

            break;

        default:
            break;
        }
    }
}

void reportCmd(int argc, char *argv[])
{
    char c;
    while ((c = getopt(argc, argv, "sclr:")) != -1)
    {
        switch (c)
        {
        case 's': //START
            doReport();
            return;

        case 'c': //CLEAN
            sendCharCommand(WRITE_R, CMD_CLEAN);
            handleBusyAndResponseReport();
            break;

        case 'l': //SHOW
            break;
        case 'r': //REMOVE
            break;

        case '?':
            if (optopt == 'r')
                printf("set: argument for 'r' not found\n");

            break;

        default:
            break;
        }
    }
}

bool handleBusyAndResponseReport()
{
    char cmd = readchar(READ_R);
    if (cmd == CMD_BUSY)
    {
        char nFile[12];
        readline(READ_R, nFile, 12);
        printf("Report is still working, %s files done\n", nFile);
        return true;
    }
    else
    {
        clearLine(READ_R);
        return cmd == RESPONSE_OK ? true : false;
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

void doReport()
{
    write(WRITE_A, "S\n", strlen("S\n"));
    if (readSimpleYNResponce(READ_A))
        printf("Report started in background\n");
    else
        printf("Impossibile avviare il report\n");
}

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

void quit(int argc, char *argv[])
{
    logg("M killed");
    exit(0);
}