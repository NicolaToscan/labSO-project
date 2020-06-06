#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../src/lib/analisys.h"

int main()
{
    int WRITE = 1;
    int READ = 0;

    int pp[2];
    pipe(pp);

    pid_t pid = fork();
    if (pid > 0) // Parent
    {
        close(pp[READ]);
        Analysis a = initAnalysis();
        a.values[0] = 123;
        a.values[8] = 456;

        printAnalysis(pp[WRITE], a);    

        exit(0);
    }
    else if (pid == 0)
    {
        close(pp[WRITE]);

        Analysis a = readAnalysis(pp[READ]);
        printf("%lu e %lu\n", a.values[0], a.values[8]);

        exit(0);
    }
    else
    {
        printf(":(\n");
    }
    
    return 0;
}