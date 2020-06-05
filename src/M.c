#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "lib/analisys.h"
#include "lib/common.h"
#include "lib/commands.h"
#include "lib/communication.h"

#define in stdin
#define out stdout

int WRITE_A = 0;
int READ_A = 0;
int PID_A = 0;

int N = 1;
int M = 1;

void startA();
void readCommand();
void file(int argc, char *argv[]);
void setCmd(int argc, char *argv[]);
void quit(int argc, char *argv[]);
void help(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    logg("M started");
    startA();

    while (true)
    {
        printf("# ");
        readCommand();
    }

    return 0;
}

void startA()
{
    int WRITE = 1;
    int READ = 0;

    int fdDOWN[2];
    pipe(fdDOWN);
    WRITE_A = fdDOWN[WRITE];

    int fdUP[2];
    pipe(fdUP);
    READ_A = fdUP[READ];

    pid_t pid = fork();
    if (pid > 0) // Parent
    {
        PID_A = pid;
        close(fdDOWN[READ]);
        close(fdUP[WRITE]);
    }
    else if (pid == 0)
    {
        close(fdDOWN[WRITE]);
        close(fdUP[READ]);

        dup2(fdDOWN[READ], STDIN_FILENO);
        dup2(fdUP[WRITE], STDOUT_FILENO);

        if (execlp(FILENAME_A, FILENAME_A, (char *)NULL) < 0)
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

void readCommand()
{
    size_t buffSize = MAX_CMD_LENGHT;
    char *inLine = NULL;
    int len = getline(&inLine, &buffSize, in);
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
        //TODO: report
    }
    else if (strcmp(cmds[0], "help") == 0)
    {
        help(num, cmds);
    }
    else if (strcmp(cmds[0], "quit") == 0)
    {
        sendKill(WRITE_A);
        quit(num, cmds);
    }
    else
    {
        printf("Command '%s' not found, type help\n", cmds[0]);
    }

    free(inLine);
    free(cmds);
}

void setCmd(int argc, char *argv[])
{

    bool nSet = false, mSet = false;
    int n = 0, m = 0;
    char c;
    while ((c = getopt(argc, argv, "n:m:")) != -1)
    {
        switch (c)
        {
        case 'n':
            nSet = true;
            n = atoi(optarg);
            break;
        case 'm':
            mSet = true;
            m = atoi(optarg);
            break;

        case '?':
            if (optopt == 'm')
                printf("set: argument for 'm' not found\n");
            else if (optopt == 'n')
                printf("set: argument for 'n' not found\n");

            break;

        default:
            break;
        }
    }

    if (nSet)
    {
        if (n <= 0)
            printf("set: argument for 'n' not valid\n");
        else
            N = n;
    }

    if (mSet)
    {
        if (m <= 0)
            printf("set: argument for 'm' not valid\n");
        else
            M = m;
    }
}

void file(int argc, char *argv[])
{
    char c;
    while ((c = getopt(argc, argv, "a:r:u:")) != -1)
    {
        switch (c)
        {
            //ADD FILE
        case 'a':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                logg(argv[optind]);
            }
            break;

            //REMOVE FILE
        case 'r':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                logg(argv[optind]);
            }
            break;

            //RECHECK FILE
        case 'u':
            optind--;
            for (; optind < argc && *argv[optind] != '-'; optind++)
            {
                logg(argv[optind]);
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

void help(int argc, char *argv[])
{
    printf("NO\n");
}

void quit(int argc, char *argv[])
{
    logg("M killed");
    exit(0);
}