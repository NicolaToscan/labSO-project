#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../src/lib/analisys.h"
#include "../src/lib/common.h"
#include "../src/lib/communication.h"

int main()
{
    int WRITE = 1;
    int READ = 0;

    int pipeToChild[2];
    int pipeToParent[2];

    pipe(pipeToChild);
    pipe(pipeToParent);

    FILE *write;
    FILE *read;

    pid_t pid = fork();
    if (pid > 0)
    {
        // Partent

        close(pipeToChild[READ]);
        close(pipeToParent[WRITE]);

        write = pipeToChild[WRITE];
        read = pipeToParent[READ];

        // Ok
        
        sendQnumbers(write, 1, 1);
        sendFilename(write, "data.txt\0", 10);

        Analysis a = readAnalysis(read);

        sendKill(write);
    }
    else if (pid == 0)
    {
        // Child

        close(pipeToChild[WRITE]);
        close(pipeToParent[READ]);

        write = pipeToParent[WRITE];
        read = pipeToChild[READ];

        dup2(read, STDIN_FILENO);
        dup2(write, STDOUT_FILENO);

        // Exec

        if(execlp(FILENAME_Q, FILENAME_Q, (char *)NULL) < 0)
        {
            printf("ERRORE EXECLP\n");
        }
    }
    else
    {
        printf(":(\n");
    }
    
    return 0;
}